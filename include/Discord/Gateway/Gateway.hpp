#pragma once



#include "Standard/Net/Websocket/Client.hpp"
#include "Heartbeat.hpp"
#include "Discord/Intent.hpp"



namespace Strawberry::Discord::Gateway
{
	class Gateway
	{
	public:
		Gateway(std::string endpoint, std::string token, Intent intents);



		Standard::Result<Standard::Net::Websocket::Message, Standard::Net::Websocket::Error> Receive();
		void                                                                                 Send(const Standard::Net::Websocket::Message& msg);



		[[nodiscard]] double GetHeartbeatInterval() const { return mHeartbeat->GetInterval(); }



	private:
		Standard::SharedMutex<Standard::Net::Websocket::WSSClient> mWSS;
		Standard::Option<Heartbeat>                                mHeartbeat;
	};
}