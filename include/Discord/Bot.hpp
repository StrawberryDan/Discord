#pragma once



#include <string>
#include <optional>
#include <set>



#include "Standard/Mutex.hpp"
#include "Standard/Net/HTTP/Client.hpp"
#include "Discord/Gateway/Gateway.hpp"
#include "Intent.hpp"
#include "Behaviour.hpp"
#include "EventListener.hpp"
#include "Event/Base.hpp"
#include "Discord/Voice/Connection.hpp"



namespace Strawberry::Discord
{
	class Bot
	{
	public:
	    using Token = std::string;

	public:
	    explicit Bot(Token token, Intent intents);



	public:
	    void Run();
		void Stop();



	public:
		void ConnectToVoice(Snowflake guild, Snowflake channel);



	public:
		const Entity::Channel* GetChannelById(const Snowflake& id) const;



	public:
		void SetBehaviour(std::unique_ptr<Behaviour> behaviour);


	public:
		void RegisterEventListener(EventListener* listener);
		void DeregisterEventListener(EventListener* listener);



	private:
		void OnGatewayMessage(Standard::Net::Websocket::Message message);



		void DispatchEvent(const Event::Base& event) const;



	private:
		void RequestVoiceInfo(Snowflake guild, Snowflake channel);



	private:
	    std::string GetGatewayEndpoint();



	private:
		bool                                                     mRunning;
	    Token                                                    mToken;
	    Intent                                                   mIntents;
		Standard::SharedMutex<Standard::Net::HTTP::HTTPSClient>  mHTTPS;
		Standard::Option<Gateway::Gateway>                       mGateway;
		std::unique_ptr<Behaviour>                               mBehaviour;
		std::set<EventListener*>                                 mEventListeners;
		Standard::Option<Snowflake>                              mUserId;


		Standard::Option<Snowflake>                              mVoiceGuild;
		Standard::Option<Snowflake>                              mVoiceChannel;
		Standard::Option<std::string>                            mVoiceEndpoint;
		Standard::Option<std::string>                            mVoiceToken;
		Standard::Option<std::string>                            mVoiceSessionId;
		Standard::Option<Voice::Connection>                      mVoiceConnection;



	private:
		std::unordered_map<Snowflake, Entity::Guild>             mGuilds;
		std::unordered_map<Snowflake, Entity::Channel>           mChannels;
	};
}