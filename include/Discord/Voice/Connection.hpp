#pragma once



#include <string>
#include "Core/Net/Websocket/Client.hpp"
#include "Core/Mutex.hpp"
#include "Core/Option.hpp"
#include "Core/Net/Socket/UDPClient.hpp"
#include "Discord/Voice/Heartbeat.hpp"
#include "Discord/Snowflake.hpp"
#include "Discord/Gateway/Gateway.hpp"



namespace Strawberry::Discord::Voice
{
	class Connection
	{
	public:
		Connection(Core::SharedMutex<Gateway::Gateway> gateway,
				   std::string sessionId,
				   Snowflake guildId,
				   Snowflake channelId,
				   Snowflake userId);

		Connection(const Connection&)				= delete;
		Connection(Connection&&)					= default;
		Connection& operator=(const Connection&)	= delete;
		Connection& operator=(Connection&&)			= delete;

		~Connection();



		void SetEndpoint(std::string endpoint);
		void SetToken(std::string token);
		void SetSessionId(std::string id);

		bool IsReady() const;
		void Start();



	private:
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
	};
}
