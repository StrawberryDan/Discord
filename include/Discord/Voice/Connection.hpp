#pragma once



#include <string>
#include "Standard/Net/Websocket/Client.hpp"
#include "Standard/Mutex.hpp"
#include "Standard/Option.hpp"
#include "Standard/Net/Socket/UDPClient.hpp"
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
		using WSS = Standard::SharedMutex<Standard::Net::Websocket::WSSClient>;
		using UDP = Standard::Option<Standard::Net::Socket::UDPClient>;
		using Key = std::array<uint8_t, 32>;
		
		
		
	private:
		WSS                         mWSS;
		Standard::Option<Heartbeat> mHeartbeat;
		UDP                         mUDP;
		Standard::Option<Key>       mKey;
	};
}
