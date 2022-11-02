#pragma once



#include <string>
#include <optional>



#include "Standard/Mutex.hpp"
#include "Standard/Net/HTTP/Client.hpp"
#include "Gateway.hpp"
#include "Intent.hpp"
#include "Behaviour.hpp"



namespace Strawberry::Discord
{
	using namespace Strawberry::Standard::Net::HTTP;



	class Bot
	{
	public:
	    using Token = std::string;

	public:
	    explicit Bot(Token token, Intent intents);

	    void Run();



		void SetBehaviour(std::unique_ptr<Behaviour> behaviour);



	private:
		void OnGatewayMessage(Message message);



	private:
	    std::string GetGatewayEndpoint();


	private:
		bool                           mRunning;
	    Token                          mToken;
	    Intent                         mIntents;
	    SharedMutex<HTTPSClient>       mHTTPS;
	    Option<Gateway>                mGateway;
		std::unique_ptr<Behaviour>     mBehaviour;
	};
}