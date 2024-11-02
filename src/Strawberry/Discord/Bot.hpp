#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Discord
#include "Strawberry/Discord/Behaviour.hpp"
#include "Strawberry/Discord/Gateway/Gateway.hpp"
#include "Strawberry/Discord/Intent.hpp"
#include "Strawberry/Discord/Voice/Connection.hpp"
#include "Strawberry/Discord/Events/MessageCreate.hpp"
// Strawberry Core
#include "Strawberry/Core/IO/ChannelBroadcaster.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
#include "Strawberry/Net/Endpoint.hpp"
#include "Strawberry/Net/HTTP/Client.hpp"
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
    using GuildList   = std::unordered_map<Snowflake, Core::Optional<Entity::Guild> >;
    using ChannelList = std::unordered_map<Snowflake, Core::Optional<Entity::Channel> >;
    using EventBroadcaster = Core::IO::ChannelBroadcaster<
        Event::Ready,
        Event::GuildCreate,
        Event::MessageCreate>;

    //==================================================================================================================
    //  Class Declaration
    //------------------------------------------------------------------------------------------------------------------
    class Bot
        : public EventBroadcaster
        , public Core::EnableReflexivePointer
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
            Core::Optional<Voice::Connection*> GetVoiceConnection()
            {
                return mVoiceConnection ? mVoiceConnection.get() : nullptr;
            }


            // Retrieves the list of known guilds this bot is a member of.
            // Fetch indicates getting the list from the server, whilst Get means using the cache.
            std::unordered_set<Snowflake> FetchGuilds();
            std::unordered_set<Snowflake> GetGuilds() const;
            // Retrieves guild data from server or from cache.
            const Entity::Guild* FetchGuild(const Snowflake& id);
            const Entity::Guild* GetGuild(const Snowflake& id) const;
            // Retrieves the channels in the given guild.
            std::unordered_set<Snowflake> FetchChannels(const Snowflake& guildId);
            std::unordered_set<Snowflake> GetChannels(const Snowflake& guildId) const;
            // Get the channel with the given ID.
            const Entity::Channel* FetchChannel(const Snowflake& id);
            const Entity::Channel* GetChannel(const Snowflake& id) const;


            void SendMessage(Snowflake channel, const std::string& message);
        private:
            // Private Constructor
            explicit Bot(Token token, Intent intents);

            template <typename... Ts> requires (fmt::is_formattable<Ts>::value && ...)
            Core::Optional<nlohmann::json> PostRequest(const nlohmann::json& json, const fmt::format_string<Ts...>& fmt, Ts&&... args)
            {
                return PostRequest(json, fmt::format(fmt, std::forward<Ts>(args)...));
            }

            Core::Optional<nlohmann::json> PostRequest(const nlohmann::json& json, const std::string& endpoint);

            // Gets a JSON entity from the discord endpoint. Template forwards arguments into fmt::format.
            template<typename... Ts> requires (fmt::is_formattable<Ts>::value && ...)
            inline Core::Optional<nlohmann::json> GetEntity(const fmt::format_string<Ts...>& endpoint, Ts&&... args)
            {
                return GetEntity(fmt::format(endpoint, std::forward<Ts>(args)...));
            }


            Core::Optional<nlohmann::json> GetEntity(const std::string& request);




            // Callback when a gateway message is received. Returns true when the message is handled. False if the message should be buffered.
            bool OnGatewayMessage(const Net::Websocket::Message& message);
            // Gets the gateway URL from HTTP.
            Core::Optional<Net::Endpoint> GetGatewayEndpoint();

        private:
            // General Utility State
            bool                                         mRunning;
            Token                                        mToken;
            Intent                                       mIntents;
            Core::SharedMutex<Net::HTTP::HTTPSClient>    mHTTPS;
            Core::SharedMutex<Gateway::Gateway>          mGateway;
            std::unique_ptr<Behaviour>                   mBehaviour;
            Core::Optional<Snowflake>                    mUserId;
            Core::Optional<std::string>                  mSessionId;

            // Voice State Info
            std::unique_ptr<Voice::Connection> mVoiceConnection;

            // Caches
            GuildList   mGuilds;
            ChannelList mChannels;
    };
} // namespace Strawberry::Discord
