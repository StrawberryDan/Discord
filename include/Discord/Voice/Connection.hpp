#pragma once



#include <string>
#include "Strawberry/Core/Net/Websocket/Client.hpp"
#include "Strawberry/Core/Mutex.hpp"
#include "Strawberry/Core/Option.hpp"
#include "Strawberry/Core/Net/Socket/UDPClient.hpp"
#include "Discord/Voice/Heartbeat.hpp"
#include "Discord/Snowflake.hpp"
#include "Discord/Gateway/Gateway.hpp"



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



	private:
		// Key is a 32 byte date slice
		using Key = std::array<uint8_t, 32>;
		
		
		
	private:
		/// Connections
		Core::SharedMutex<Gateway::Gateway>					mGateway;
		Core::SharedMutex<Core::Net::Websocket::WSSClient>	mVoiceWSS;
		Core::Option<Heartbeat>								mVoiceWSSHeartbeat;
		Core::Option<Core::Net::Endpoint>					mUDPVoiceEndpoint;
		Core::Option<Core::Net::Socket::UDPClient>			mUDPVoiceConnection;
		Core::Option<Key>									mSodiumEncryptionKey;

		/// Current Voice Channels and User
		Snowflake											mGuild;
		Snowflake											mChannel;
		Snowflake											mUser;

		/// SSRC for our RTP Connection
		Core::Option<uint32_t>								mSSRC;

		bool												mIsSpeaking = false;
	};
}
