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
				   std::string sessionId,
				   Snowflake guildId,
				   Snowflake channelId,
				   Snowflake userId);

		/// Only move construction allowed.
		Connection(const Connection&)				= delete;
		Connection(Connection&&)					= default;
		Connection& operator=(const Connection&)	= delete;
		Connection& operator=(Connection&&)			= delete;
		~Connection();


		/// Setters for state that is sent from the gateway after construction.
		void SetEndpoint(std::string endpoint);
		void SetToken(std::string token);
		void SetSessionId(std::string id);

		bool IsReady() const;
		void Start();



	private:
		// Key is a 32 byte date slice
		using Key = std::array<uint8_t, 32>;
		
		
		
	private:
		Core::SharedMutex<Gateway::Gateway>					mGateway;
		Core::SharedMutex<Core::Net::Websocket::WSSClient>	mWSS;
		Core::Option<Heartbeat>								mHeartbeat;
		Core::Option<Core::Net::Socket::UDPClient>			mUDP;
		Core::Option<Key>									mKey;

		Snowflake											mGuild;
		Snowflake											mChannel;
		Snowflake											mUser;
		Core::Option<std::string>							mEndpoint;
		Core::Option<std::string>							mToken;
		std::string											mSessionId;
		Core::Option<uint32_t>								mSSRC;
	};
}
