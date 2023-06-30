//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Discord/Voice/Connection.hpp"
/// Strawberry Libraries
#include "nlohmann/json.hpp"
#include "Strawberry/Core/Net/Address.hpp"
#include "Strawberry/Core/Net/Endpoint.hpp"
#include "Strawberry/Core/Net/RTP/Packet.hpp"
/// Standard Library
#include <utility>



namespace Strawberry::Discord::Voice
{
	Connection::Connection(Core::SharedMutex<Gateway::Gateway> gateway,
						   const std::string& sessionId,
						   Snowflake guildId,
						   Snowflake channelId,
						   Snowflake userId)
		: mGateway(std::move(gateway))
		, mVoiceWSS(nullptr)
		, mGuild(guildId)
		, mChannel(channelId)
		, mUser(userId)
	{
		using nlohmann::json;
		using namespace Core::Net::Websocket;

		auto gatewayLock = mGateway.Lock();

        // Send the voice state update to tell discord we're joining a channel.
		json voiceStateUpdate;
        voiceStateUpdate["op"]				= 4;
        voiceStateUpdate["d"]["guild_id"]	= mGuild.AsString();
        voiceStateUpdate["d"]["channel_id"]	= mChannel.AsString();
        voiceStateUpdate["d"]["self_mute"]	= false;
        voiceStateUpdate["d"]["self_deaf"]	= false;
		gatewayLock->Send(voiceStateUpdate).Unwrap();

		while (true)
		{
			auto message = gatewayLock->Receive(false);
			if (!message && message.Err() == Core::Net::Websocket::Error::NoMessage)
			{
				std::this_thread::yield();
				continue;
			}

			auto messageAsJsonResult = message->AsJSON();
			if (!messageAsJsonResult)
			{
				gatewayLock->BufferMessage(std::move(*message));
				continue;
			}

			auto voiceServerUpdate = messageAsJsonResult.Unwrap();
			if (voiceServerUpdate["t"] != "VOICE_SERVER_UPDATE")
			{
				gatewayLock->BufferMessage(std::move(*message));
				continue;
			}

			auto voiceWSSHost = Core::Net::Endpoint::Parse(voiceServerUpdate["d"]["endpoint"]).Unwrap();

			// Start websocket
			mVoiceWSS.Emplace(WSSClient::Connect(*voiceWSSHost.GetHostname(), "/?v=4").Unwrap());
			// Identify with the voice server
			json identification;
			identification["op"]				= 0;
			identification["d"]["server_id"]	= mGuild.AsString();
			identification["d"]["user_id"]		= mUser.AsString();
			identification["d"]["session_id"]	= sessionId;
			identification["d"]["token"]		= voiceServerUpdate["d"]["token"];
			auto voiceWSS = mVoiceWSS.Lock();
			voiceWSS->SendMessage(identification).Unwrap();

			// Receive Hello
			auto helloMessage = voiceWSS->WaitMessage().Unwrap().AsJSON().Unwrap();
			auto heartbeatInterval = static_cast<double>(helloMessage["d"]["heartbeat_interval"]) / 1000.0;
			mVoiceWSSHeartbeat.Emplace(mVoiceWSS, heartbeatInterval);

			// Receive Voice Ready Message
			auto ready = voiceWSS->WaitMessage().Unwrap().AsJSON().Unwrap();
			Core::Assert(ready["op"] == 2);
			// Parse SSRC
			mSSRC = ready["d"]["ssrc"];
			// Parse UDP Endpoint
			mUDPVoiceEndpoint.Emplace(
					Core::Net::IPv4Address::Parse(ready["d"]["ip"]).Unwrap(),
					ready["d"]["port"]);
			// Get list of available modes
			std::vector<std::string> modes = ready["d"]["modes"];
			const auto voiceMode = "xsalsa20_poly1305";
			// Check that the mode we want is there
			Core::Assert(std::find(modes.begin(), modes.end(), voiceMode) != modes.end());
			mUDPVoiceConnection = Core::Net::Socket::UDPClient::Create().Unwrap();

			// Send protocol selection
			nlohmann::json protocolSelect;
			protocolSelect["op"] = 1;
			protocolSelect["d"]["protocol"] = "udp";
			protocolSelect["d"]["data"]["address"] = mUDPVoiceEndpoint->GetAddress()->AsString();
			protocolSelect["d"]["data"]["port"] = mUDPVoiceEndpoint->GetPort();
			protocolSelect["d"]["data"]["mode"] = voiceMode;
			voiceWSS->SendMessage(protocolSelect).Unwrap();

			// Receive session description
			auto sessionDescription = voiceWSS->WaitMessage().Unwrap().AsJSON().Unwrap();
			Core::Assert(sessionDescription["op"] == 4);
			Core::Assert(sessionDescription["d"]["mode"] == voiceMode);
			mSodiumEncryptionKey = sessionDescription["d"]["secret_key"];

			break;
		}

		*mVoiceSendingThreadShouldRun.Lock() = true;
		mVoiceSendingThread.Emplace([this]()
		{
			while (*mVoiceSendingThreadShouldRun.Lock())
			{
				bool packetsAvailable = !mVoicePacketBuffer.Lock()->Empty();
				if (!packetsAvailable)
				{
					if (!mIsSpeaking)
					{
						SetSpeaking(false);
					}

					std::this_thread::yield();
					continue;
				}

				if (*mTimeSinceLastVoicePacketSent > (0.0020 * 0.95)) {
					Core::Assert(!mVoicePacketBuffer.Lock()->Empty());
					auto packet = mVoicePacketBuffer.Lock()->Pop().Unwrap();

					Core::Net::RTP::Packet rtpPacket(78, ++mLastSequenceNumber, (mLastTimestamp += 20), *mSSRC);
					rtpPacket.SetPayload(packet.AsBytes());
					mUDPVoiceConnection->Write(*mUDPVoiceEndpoint, rtpPacket.AsBytes()).Unwrap();
					mTimeSinceLastVoicePacketSent.Restart();
				}
			}
		});
	}



	Connection::~Connection()
	{
		using nlohmann::json;
		using namespace Core::Net::Websocket;

		json request;
		request["op"]				= 4;
		request["d"]["guild_id"]	= mGuild.AsString();
		request["d"]["channel_id"]	= {};
		request["d"]["self_mute"]	= false;
		request["d"]["self_deaf"]	= false;

		Message msg(request.dump());
		mGateway.Lock()->Send(msg).Unwrap();

		if (mVoiceSendingThread)
		{
			*mVoiceSendingThreadShouldRun.Lock() = false;
			mVoiceSendingThread->join();
		}
	}


	void Connection::SendAudioPacket(const Codec::Packet &packet)
	{
		if (!mIsSpeaking) SetSpeaking(true);

		mVoicePacketBuffer.Lock()->Push(packet);
	}


	void Connection::SetSpeaking(bool speaking)
	{
		if (speaking && !mIsSpeaking)
		{
			nlohmann::json speaking;
			speaking["op"]            = 5;
			speaking["d"]["speaking"] = 1;
			speaking["d"]["delay"]    = 0;
			speaking["d"]["ssrc"]     = *mSSRC;
			mVoiceWSS.Lock()->SendMessage(speaking).Unwrap();
		}
		else if (!speaking && mIsSpeaking)
		{
			nlohmann::json speaking;
			speaking["op"]            = 5;
			speaking["d"]["speaking"] = 0;
			speaking["d"]["delay"]    = 0;
			speaking["d"]["ssrc"]     = *mSSRC;
			mVoiceWSS.Lock()->SendMessage(speaking).Unwrap();
		}

		mIsSpeaking = speaking;
	}


	void Connection::ClearAudioPackets()
	{
		mVoicePacketBuffer.Lock()->Clear();
	}
}