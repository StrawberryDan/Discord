#pragma once



#include "Core/Net/Websocket/Client.hpp"
#include "Heartbeat.hpp"
#include "Discord/Intent.hpp"



namespace Strawberry::Discord::Gateway
{
	class Gateway
	{
	public:
		Gateway(std::string endpoint, std::string token, Intent intents);



		Core::Result<Core::Net::Websocket::Message, Core::Net::Websocket::Error> Receive();
		void                                                                                 Send(const Core::Net::Websocket::Message& msg);



		[[nodiscard]] double GetHeartbeatInterval() const { return mHeartbeat->GetInterval(); }



	private:
		Core::SharedMutex<Core::Net::Websocket::WSSClient> mWSS;
		Core::Option<Heartbeat>                                mHeartbeat;
	};
}