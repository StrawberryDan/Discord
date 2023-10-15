#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Discord
#include "Discord/Behaviour.hpp"
#include "Discord/Event/EventBase.hpp"
#include "Discord/EventListener.hpp"
#include "Discord/Gateway/Gateway.hpp"
#include "Discord/Intent.hpp"
#include "Discord/Voice/Connection.hpp"
// Strawberry Core
#include "Strawberry/Core/Net/Endpoint.hpp"
#include "Strawberry/Core/Net/HTTP/Client.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
// C++ Standard Library
#include <concepts>
#include <optional>
#include <set>
#include <string>
#include <unordered_set>

namespace Strawberry::Discord
{
	//==================================================================================================================
	//  Type Aliases
	//------------------------------------------------------------------------------------------------------------------
	using GuildList   = std::unordered_map<Snowflake, Core::Optional<Entity::Guild>>;
	using ChannelList = std::unordered_map<Snowflake, Core::Optional<Entity::Channel>>;

	//==================================================================================================================
	//  Class Declaration
	//------------------------------------------------------------------------------------------------------------------
	class Bot
	{
	public:
		using Token = std::string;

	public:
		static Core::Optional<Bot> Connect(Token token, Intent intents);

		Bot(Bot&&) = default;

		/// Returns whether the bot is okay to run.
		bool IsOk() const;
		/// Runs the bot. Does not start a new thread. Can be called from another thread.
		void Run();
		/// Sets the flag so that Run will return as soon as possible.
		void Shutdown();
		/// Returns whether the bot is running or not.
		bool IsRunning() const;
		/// Set the behaviour to use whilst the Bot is running.
		/// This Must be called whilst the bot is not running.
		void SetBehaviour(std::unique_ptr<Behaviour> behaviour);


		// Connects the bot to the given channel in the given guild.
		void ConnectToVoice(Snowflake guild, Snowflake channel);
		// Disconnects the bot from any voice channel.
		void DisconnectFromVoice();

		// Accessor for the voice connection.
		Core::Optional<Voice::Connection*> GetVoiceConnection() { return mVoiceConnection ? mVoiceConnection.get() : nullptr; }

		// Retrieves the list of known guilds this bot is a member of.
		// Fetch indicates getting the list from the server, whilst Get means using the cache.
		std::unordered_set<Snowflake> FetchGuilds();
		std::unordered_set<Snowflake> GetGuilds() const;
		// Retrieves guild data from server or from cache.
		const Entity::Guild*          FetchGuild(const Snowflake& id);
		const Entity::Guild*          GetGuild(const Snowflake& id) const;
		// Retrieves the channels in the given guild.
		std::unordered_set<Snowflake> FetchChannels(const Snowflake& guildId);
		std::unordered_set<Snowflake> GetChannels(const Snowflake& guildId) const;
		// Get the channel with the given ID.
		const Entity::Channel*        FetchChannel(const Snowflake& id);
		const Entity::Channel*        GetChannel(const Snowflake& id) const;

		// Register an EventListener with this bot.
		void RegisterEventListener(EventListener* listener);
		// Unregister an event listener with this bot.
		void DeregisterEventListener(EventListener* listener);


	private:
		// Private Constructor
		explicit Bot(Token token, Intent intents);

		// Gets a JSON entity from the discord endpoint. Template forwards arguments into fmt::format.
		template <typename... Ts>
			requires std::same_as<std::string, decltype(fmt::format(std::declval<std::string>(), std::declval<Ts>()...))>
		inline Core::Optional<nlohmann::json> GetEntity(const std::string& endpoint, Ts... args)
		{
			return GetEntity(fmt::format(fmt::runtime(endpoint), std::forward<Ts>(args)...));
		}

		// Callback when a gateway message is received. Returns true when the message is handled. False if the message should be buffered.
		bool                        OnGatewayMessage(const Core::Net::Websocket::Message& message);
		// Dispatches an event to all event listeners.
		void                        DispatchEvent(const Event::EventBase& event) const;
		// Gets the gateway URL from HTTP.
		Core::Optional<Core::Net::Endpoint> GetGatewayEndpoint();


	private:
		// General Utility State
		bool                                            mRunning;
		Token                                           mToken;
		Intent                                          mIntents;
		Core::SharedMutex<Core::Net::HTTP::HTTPSClient> mHTTPS;
		Core::SharedMutex<Gateway::Gateway>             mGateway;
		std::unique_ptr<Behaviour>                      mBehaviour;
		Core::SharedMutex<std::set<EventListener*>>     mEventListenerRegistry;
		Core::Optional<Snowflake>                       mUserId;
		Core::Optional<std::string>                     mSessionId;

		// Voice State Info
		std::unique_ptr<Voice::Connection> mVoiceConnection;

		// Caches
		GuildList   mGuilds;
		ChannelList mChannels;
	};

	// Base case for GetEntity which actually does the request.
	template <>
	Core::Optional<nlohmann::json> Bot::GetEntity(const std::string& endpoint);
} // namespace Strawberry::Discord