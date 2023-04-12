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
#include "Event/EventBase.hpp"
#include "Discord/Voice/Connection.hpp"



namespace Strawberry::Discord
{
	class Bot
	{
	public:
		using Token = std::string;

	public:
		explicit Bot(Token token, Intent intents);


		// Runs the bot. Does not start a new thread. Can be called from another thread however.
		void Run();
		// Sets the flag so that Run will return as soon as possible.
		void Stop();
		// Returns whether the bot is running or not.
		bool IsRunning() const;
		// Set the behaviour to use whilst the Bot is running.
		// This Must be called whilst the bot is not running.
		void SetBehaviour(std::unique_ptr<Behaviour> behaviour);


		// Connects the bot to the given channel in the given guild.
		void ConnectToVoice(Snowflake guild, Snowflake channel);

		// Get the channel with the given ID if it is cached.
		// Otherwise, return nullptr if the channel is unknown to us.
		const Entity::Channel* GetChannelById(const Snowflake& id) const;

		// Register an EventListener with this bot.
		void RegisterEventListener(EventListener* listener);
		// Unregister an event listener with this bot.
		void DeregisterEventListener(EventListener* listener);



	private:
		// Callback when a gateway message is received.
		void			OnGatewayMessage(Core::Net::Websocket::Message message);
		// Dispatches an event to all event listeners.
		void			DispatchEvent(const Event::EventBase& event) const;
		// Sends a request for voice server information for the given channel.
		void			RequestVoiceInfo(Snowflake guild, Snowflake channel);
		// Gets the gateway URL from HTTP.
		std::string		GetGatewayEndpoint();



	private:
		// General Utility State
		bool											mRunning;
		Token											mToken;
		Intent											mIntents;
		Core::SharedMutex<Core::Net::HTTP::HTTPSClient>	mHTTPS;
		Core::Option<Gateway::Gateway>					mGateway;
		std::unique_ptr<Behaviour>						mBehaviour;
		Core::SharedMutex<std::set<EventListener*>>		mEventListenerRegistry;
		Core::Option<Snowflake>							mUserId;
		Core::Option<std::string>						mSessionId;


		/// Voice State Info
		Core::Option<Snowflake>							mVoiceGuild;
		Core::Option<Snowflake>							mVoiceChannel;
		Core::Option<std::string>						mVoiceEndpoint;
		Core::Option<std::string>						mVoiceToken;
		Core::Option<std::string>						mVoiceSessionId;
		Core::Option<Voice::Connection>					mVoiceConnection;

		// Cache
		std::unordered_map<Snowflake, Entity::Guild>	mGuilds;
		std::unordered_map<Snowflake, Entity::Channel>	mChannels;
	};
}