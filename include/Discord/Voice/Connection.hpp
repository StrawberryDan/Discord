#pragma once



#include <string>
#include "Core/Net/Websocket/Client.hpp"
#include "Core/Mutex.hpp"
#include "Core/Option.hpp"
#include "Core/Net/Socket/UDPClient.hpp"
#include "Discord/Voice/Heartbeat.hpp"
#include "Discord/Snowflake.hpp"



namespace Strawberry::Discord::Voice
{
	class Connection
	{
	public:
		Connection(std::string endpoint,
				   std::string sessionId,
				   std::string token,
				   Snowflake guildId,
				   Snowflake userId);



	private:
		using Key = std::array<uint8_t, 32>;
		
		
		
	private:
		Core::SharedMutex<Core::Net::Websocket::WSSClient>	mWSS;
		Core::Option<Heartbeat>								mHeartbeat;
		Core::Option<Core::Net::Socket::UDPClient>			mUDP;
		Core::Option<Key>									mKey;
	};
}
