#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
/// Strawberry Libraries
#include "Codec/Packet.hpp"
#include "Codec/SodiumEncrypter.hpp"
#include "Discord/Gateway/Gateway.hpp"
#include "Discord/Snowflake.hpp"
#include "Discord/Voice/Heartbeat.hpp"
#include "Strawberry/Core/Collection/CircularBuffer.hpp"
#include "Strawberry/Core/Mutex.hpp"
#include "Strawberry/Core/Net/Socket/UDPClient.hpp"
#include "Strawberry/Core/Net/Websocket/Client.hpp"
#include "Strawberry/Core/Option.hpp"
#include "Strawberry/Core/Net/RTP/Packet.hpp"

/// Standard Library
#include <string>



namespace Strawberry::Discord::Voice
{
	/// Class for representing a voice connection for a discord bot.
	class Connection
	{
	public:
		Connection(Core::SharedMutex<Gateway::Gateway> gateway,
				   const std::string& sessionId,
				   Snowflake guildId,
				   Snowflake channelId,
				   Snowflake userId);

		/// Only move construction allowed.
		Connection(const Connection&)				= delete;
		Connection(Connection&&)					= default;
		Connection& operator=(const Connection&)	= delete;
		Connection& operator=(Connection&&)			= delete;
		~Connection();


		void SetSpeaking(bool speaking);
		void SendAudioPacket(const Codec::Packet& packet);
		void ClearAudioPackets();



	private:
		// Key is a 32 byte date slice
		using Key = std::array<uint8_t, 32>;



	private:
		/// Connections
		Core::SharedMutex<Gateway::Gateway>							mGateway;
		Core::SharedMutex<Core::Net::Websocket::WSSClient>			mVoiceWSS;
		Core::Option<Heartbeat>										mVoiceWSSHeartbeat;
		Core::Option<Core::Net::Endpoint>							mUDPVoiceEndpoint;
		Core::Option<Core::Net::Socket::UDPClient>					mUDPVoiceConnection;
		Core::Option<Codec::SodiumEncrypter>						mSodiumEncrypter;


		/// Current Voice Channels and User
		Snowflake													mGuild;
		Snowflake													mChannel;
		Snowflake													mUser;

		/// SSRC for our RTP Connection
		Core::Option<uint32_t>										mSSRC;

		/// Whether the client is considered to be speaking. Corresponds to the speaking messaged in Discord API.
		bool													mIsSpeaking = false;

		/// Voice Packet Buffer
		Core::Mutex<Core::Collection::DynamicCircularBuffer<Core::Net::RTP::Packet>>	mVoicePacketBuffer;

		/// Voice Sending Thread
		Core::Clock												mTimeSinceLastVoicePacketSent;
		Core::Option<std::thread>								mVoiceSendingThread;
		Core::Mutex<bool>										mVoiceSendingThreadShouldRun;
		uint32_t 												mLastSequenceNumber = 0;
		uint32_t 												mLastTimestamp      = 0;
	};
}
