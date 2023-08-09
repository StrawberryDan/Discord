//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Discord/Voice/Connection.hpp"
/// Strawberry Libraries
#include "Strawberry/Core/Net/Address.hpp"
#include "Strawberry/Core/Net/Endpoint.hpp"
#include "Strawberry/Core/Net/RTP/Packet.hpp"
#include "Strawberry/Core/Util/Logging.hpp"
#include "Strawberry/Core/Util/Metronome.hpp"
// Json
#include "nlohmann/json.hpp"
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
		, mAudioMixer({48000, AV_SAMPLE_FMT_S32, AV_CHANNEL_LAYOUT_STEREO}, 960)
		, mOpusEncoder(AV_CODEC_ID_OPUS, AV_CHANNEL_LAYOUT_STEREO)
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
			Core::Logging::Debug("Received voice ready message:\n{}", ready.dump(1, '\t'));
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
			mUDPVoiceConnection = Core::Net::Socket::UDPClient::CreateIPv4().Unwrap();

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
			while (sessionDescription["op"] != 4)
			{
				sessionDescription = voiceWSS->WaitMessage().Unwrap().AsJSON().Unwrap();
			}


			Core::Assert(sessionDescription["op"] == 4);
			Core::Logging::Debug("Voice Session Description Update:\n{}", sessionDescription.dump(1, '\t'));
			Core::Assert(sessionDescription["d"]["mode"] == voiceMode);
			mSodiumEncrypter.Emplace(sessionDescription["d"]["secret_key"]);

			break;
		}


		mVoiceSendingThread.Emplace([this, clock = Core::Metronome(0.02, 0.06), silentSamplesSent = 0]() mutable
		{
			if (clock)
			{
				clock.Tick();

				Core::Option<Codec::Audio::Frame> frame;
				if (mAudioMixer.IsEmpty())
				{
					if (mIsSpeaking && silentSamplesSent < 5)
					{
						frame = Codec::Audio::Frame::Silence({48000, AV_SAMPLE_FMT_S32, AV_CHANNEL_LAYOUT_STEREO},
															 960);
						silentSamplesSent += 1;
					}
					else
					{
						SetSpeaking(false);
					}
				}
				else
				{
					SetSpeaking(true);
					silentSamplesSent = 0;
					frame = mAudioMixer.ReadFrame();
				}


				if (frame)
				{
					Core::Assert(mIsSpeaking);
					mOpusEncoder.Send(frame.Unwrap());
					for (auto packet: mOpusEncoder.Receive())
					{
						Core::IO::DynamicByteBuffer packetData(packet->data, packet->size);
						Core::Net::RTP::Packet rtpPacket(0x78, mLastSequenceNumber++, mLastTimestamp, *mSSRC);
						mLastTimestamp += packet->duration;
						Codec::SodiumEncrypter::Nonce nonce{};
						auto rtpAsBytes = rtpPacket.AsBytes();
						for (int i = 0; i < sizeof(Core::Net::RTP::Packet::Header); i++) nonce[i] = rtpAsBytes[i];
						rtpPacket.SetPayload(mSodiumEncrypter->Encrypt(nonce, packetData).second);
						rtpAsBytes = rtpPacket.AsBytes();
						Core::Assert(rtpAsBytes[0] == 0x80);
						mUDPVoiceConnection->Write(*mUDPVoiceEndpoint, rtpAsBytes).Unwrap();
					}
				}
			}
		});
	}



	Connection::~Connection()
	{
		using nlohmann::json;
		using namespace Core::Net::Websocket;

		SetSpeaking(false);

		json request;
		request["op"]				= 4;
		request["d"]["guild_id"]	= mGuild.AsString();
		request["d"]["channel_id"]	= {};
		request["d"]["self_mute"]	= false;
		request["d"]["self_deaf"]	= false;

		Message msg(request.dump());
		mGateway.Lock()->Send(msg).Unwrap();
	}


	std::shared_ptr<Codec::Audio::Mixer::InputChannel> Connection::CreateInputChannel()
	{
		return mAudioMixer.CreateInputChannel();
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
			Core::Logging::Trace("{}:{}\tSent Start Speaking Message", __FILE__, __LINE__);
		}
		else if (!speaking && mIsSpeaking)
		{
			nlohmann::json speaking;
			speaking["op"]            = 5;
			speaking["d"]["speaking"] = 0;
			speaking["d"]["delay"]    = 0;
			speaking["d"]["ssrc"]     = *mSSRC;
			mVoiceWSS.Lock()->SendMessage(speaking).Unwrap();
			Core::Logging::Trace("{}:{}\tSent Stop Speaking Message", __FILE__, __LINE__);
		}

		mIsSpeaking = speaking;
	}
}
