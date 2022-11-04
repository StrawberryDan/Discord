#pragma once



#include <string>
#include "Standard/Net/Websocket/Client.hpp"
#include "Standard/Mutex.hpp"
#include "Standard/Option.hpp"
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
		Standard::SharedMutex<Standard::Net::Websocket::WSSClient> mWSS;
		Standard::Option<Heartbeat>                                mHeartbeat;




	};
}
