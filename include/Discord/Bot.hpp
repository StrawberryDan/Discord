#pragma once



#include <string>
#include <optional>
#include <set>



#include "Standard/Mutex.hpp"
#include "Standard/Net/HTTP/Client.hpp"
#include "Gateway.hpp"
#include "Intent.hpp"
#include "Behaviour.hpp"
#include "EventListener.hpp"
#include "Event/Base.hpp"



namespace
{
	using Strawberry::Standard::Net::HTTP::HTTPSClient;
}



namespace Strawberry::Discord
{
	class Bot
	{
	public:
	    using Token = std::string;

	public:
	    explicit Bot(Token token, Intent intents);



	    void Run();



		void SetBehaviour(std::unique_ptr<Behaviour> behaviour);



		void RegisterEventListener(EventListener* listener);

		void DeregisterEventListener(EventListener* listener);



	private:
		void OnGatewayMessage(Message message);



		void DispatchEvent(const Event::Base& event) const;



	private:
	    std::string GetGatewayEndpoint();


	private:
		bool                           mRunning;
	    Token                          mToken;
	    Intent                         mIntents;
	    SharedMutex<HTTPSClient>       mHTTPS;
	    Option<Gateway>                mGateway;
		std::unique_ptr<Behaviour>     mBehaviour;
		std::set<EventListener*>       mEventListeners;
	};
}