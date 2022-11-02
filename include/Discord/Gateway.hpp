#pragma once



#include "Standard/Net/Websocket/Client.hpp"
#include "Heartbeat.hpp"
#include "Discord/Intent.hpp"



namespace Strawberry::Discord
{
	namespace
	{
		using Standard::Result;
		using Standard::Net::Websocket::Message;
	}



	class Gateway
	{
	public:
		Gateway(std::string endpoint, std::string token, Intent intents);

		Result<Message, WSSClient::Error> Receive();
		void                              Send(const Message& msg);
	private:
		SharedMutex<WSSClient>        mWSS;
		Option<Heartbeat>             mHeartbeat;
	};
}