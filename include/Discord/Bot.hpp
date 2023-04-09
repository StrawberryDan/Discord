#pragma once



#include <string>
#include <optional>
#include <set>



#include "Core/Mutex.hpp"
#include "Core/Net/HTTP/Client.hpp"
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
		void OnGatewayMessage(Core::Net::Websocket::Message message);



		void DispatchEvent(const Event::Base& event) const;



	private:
		void RequestVoiceInfo(Snowflake guild, Snowflake channel);



	private:
	    std::string GetGatewayEndpoint();



	private:
		bool											mRunning;
	    Token											mToken;
	    Intent											mIntents;
		Core::SharedMutex<Core::Net::HTTP::HTTPSClient>	mHTTPS;
		Core::Option<Gateway::Gateway>					mGateway;
		std::unique_ptr<Behaviour>						mBehaviour;
		std::set<EventListener*>						mEventListeners;
		Core::Option<Snowflake>							mUserId;


		Core::Option<Snowflake>							mVoiceGuild;
		Core::Option<Snowflake>							mVoiceChannel;
		Core::Option<std::string>						mVoiceEndpoint;
		Core::Option<std::string>						mVoiceToken;
		Core::Option<std::string>						mVoiceSessionId;
		Core::Option<Voice::Connection>					mVoiceConnection;



	private:
		std::unordered_map<Snowflake, Entity::Guild>	mGuilds;
		std::unordered_map<Snowflake, Entity::Channel>	mChannels;
	};
}