#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Codec
#include "Codec/Audio/Mixer.hpp"
#include "Codec/Packet.hpp"
#include "Codec/SodiumEncrypter.hpp"
// Discord
#include "Discord/Gateway/Gateway.hpp"
#include "Discord/Snowflake.hpp"
#include "Discord/Voice/Heartbeat.hpp"
// Core
#include "Strawberry/Core/Collection/CircularBuffer.hpp"
#include "Strawberry/Core/LoopingThread.hpp"
#include "Strawberry/Core/Mutex.hpp"
#include "Strawberry/Core/Net/RTP/Packet.hpp"
#include "Strawberry/Core/Net/Socket/UDPClient.hpp"
#include "Strawberry/Core/Net/Websocket/Client.hpp"
#include "Strawberry/Core/Option.hpp"
// Standard Library
#include <string>
#include <Codec/Audio/Encoder.hpp>


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


		std::shared_ptr<Codec::Audio::Mixer::InputChannel> CreateInputChannel();


	protected:
		void SetSpeaking(bool speaking);



	private:
		// Key is a 32 byte date slice
		using Key = std::array<uint8_t, 32>;



	private:
		static constexpr double kAllowedAheadTime = 0.02;
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
		Codec::Audio::Mixer   mAudioMixer;
		Codec::Audio::Encoder mOpusEncoder;

		/// Voice Sending Thread
		Core::Clock												mTimeSinceLastVoicePacketSent;
		Core::Option<Core::LoopingThread>						mVoiceSendingThread;
		uint32_t 												mLastSequenceNumber = 0;
		uint32_t 												mLastTimestamp      = 0;
	};
}
