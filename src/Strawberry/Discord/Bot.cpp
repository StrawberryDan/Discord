#include <utility>

#include "Strawberry/Discord/Bot.hpp"
#include "Strawberry/Discord/Events/GuildCreate.hpp"
#include "Strawberry/Core/IO/Logging.hpp"


using namespace Strawberry::Net;
using Strawberry::Core::Assert;
using Strawberry::Core::Unreachable;
using Strawberry::Net::HTTP::HTTPSClient;

namespace Strawberry::Discord
{
    Core::Optional<Bot> Bot::Connect(Token token, Intent intents)
    {
        Bot bot(std::move(token), std::move(intents));

        if (!bot.IsOk()) return Core::NullOpt;

        return bot;
    }


    Bot::Bot(Token token, Intent intents)
        : mRunning(true)
        , mToken(std::move(token))
        , mIntents(intents)
        , mHTTPS(Endpoint::Resolve("discord.com", 443).Unwrap())
        , mGateway(nullptr)
    {
        auto endpoint = GetGatewayEndpoint().Unwrap();
        auto gateway  = Gateway::Gateway::Connect(endpoint, mToken, mIntents);
        if (gateway) mGateway = Core::SharedMutex(gateway.Unwrap());
    }


    bool Bot::IsOk() const
    {
        return mGateway;
    }


    void Bot::Run()
    {
        mRunning = true;
        while (mRunning)
        {
            auto gateway = mGateway.Lock();

            if (!gateway->IsOk())
            {
                mGateway.Lock()->Resume();
            }

            auto gatewayMessage = gateway->Receive();
            if (gatewayMessage)
            {
                if (gatewayMessage->GetOpcode() == Net::Websocket::Message::Opcode::Close)
                {
                    Core::Logging::Info("Websocket server closed with: {}", gatewayMessage->GetCloseStatusCode());
                }

                if (!OnGatewayMessage(*gatewayMessage))
                {
                    gateway->BufferMessage(*gatewayMessage);
                }
            }
            else
            {
                switch (gatewayMessage.Err())
                {
                    case Net::Error::NoData: std::this_thread::yield();
                        continue;
                    case Net::Error::ConnectionReset: mRunning = false;
                        break;
                    case Net::Error::ProtocolError: continue;
                    default: mGateway.Lock()->Resume();
                        break;
                }
            }


            if (mVoiceConnection && !mVoiceConnection->IsOk())
            {
                auto guild   = mVoiceConnection->GetGuild();
                auto channel = mVoiceConnection->GetChannel();
                DisconnectFromVoice();
                ConnectToVoice(guild, channel);
            }
        }
    }


    bool Bot::OnGatewayMessage(const Websocket::Message& message)
    {
        auto json = message.AsJSON().UnwrapOr({});
        if (json.is_null())
        {
            return false;
        }

        switch (static_cast<int>(json["op"]))
        {
            case 0: // Update Event
            {
                const std::string type = json["t"];
                if (type == "READY")
                {
                    Event::Ready event = Event::Ready::Parse(json).Unwrap();
                    mUserId            = event.GetUserId();
                    mSessionId         = event.GetSessionId();
                    mGateway.Lock()->SetResumeGatewayURL(event.GetResumeGatewayURL());
                    if (mBehaviour) mBehaviour->OnReady(event);
                    Broadcast(event);
                    return true;
                }
                else if (json["t"] == "GUILD_CREATE")
                {
                    auto event = Event::GuildCreate::Parse(json).Unwrap();

                    // Cache guilds and Channels
                    mGuilds.insert_or_assign(event.GetGuild().GetId(), event.GetGuild());

                    // Action event
                    if (mBehaviour) mBehaviour->OnGuildCreate(event);
                    Broadcast(event);

                    return true;
                }
                else if (json["t"] == "MESSAGE_CREATE")
                {
                    Event::MessageCreate event = Event::MessageCreate::Parse(json).Unwrap();
                    Broadcast(event);
                }
                else if (json["t"] == "VOICE_SERVER_UPDATE")
                {
                    return false;
                }
                else if (json["t"] == "VOICE_STATE_UPDATE")
                {
                    // Print for later debugging purposes.
                    Core::Logging::Debug("Voice State Update:\n{}", json["d"].dump(1, '\t'));
                    return true;
                }
                else
                {
                    Core::Logging::Debug("Unhandled Discord Websocket update event:\n{}", json.dump(1, '\t'));
                    return true;
                }
            }

            case 4:


            case 11: // Heartbeat Acknowledge
                return true;

            default: Core::Logging::Warning("Unhandled Discord Websocket OpCode:\n{}", json.dump(1, '\t'));
                return false;
        }
    }


    void Bot::Shutdown()
    {
        mRunning = false;
    }


    bool Bot::IsRunning() const
    {
        return mRunning;
    }


    void Bot::SetBehaviour(std::unique_ptr<Behaviour> behaviour)
    {
        Core::Assert(!IsRunning());
        mBehaviour = std::move(behaviour);
    }


    void Bot::ConnectToVoice(Snowflake guild, Snowflake channel)
    {
        mVoiceConnection.reset(new Voice::Connection(mGateway, *mSessionId, guild, channel, *mUserId));
    }


    void Bot::DisconnectFromVoice()
    {
        mVoiceConnection.reset();
    }


    std::unordered_set<Snowflake> Bot::FetchGuilds()
    {
        std::unordered_set<Snowflake> result;
        nlohmann::json                responseJSON = GetEntity("/users/@me/guilds").Unwrap();
        Core::Assert(responseJSON.is_array());
        // Add to list of known guilds.
        for (const auto& guildJSON: responseJSON)
        {
            auto guild = Entity::Guild::Parse(guildJSON).Unwrap();
            result.insert(guild.GetId());

            // Cache guild id.
            if (!mGuilds.contains(guild.GetId()))
            {
                mGuilds.insert({guild.GetId(), guild});
            }
        }

        return result;
    }


    std::unordered_set<Snowflake> Bot::GetGuilds() const
    {
        std::unordered_set<Snowflake> result;
        result.reserve(mGuilds.size());

        for (auto& [id, data]: mGuilds)
        {
            result.insert(id);
        }

        return result;
    }


    const Entity::Guild* Bot::FetchGuild(const Snowflake& id)
    {
        auto guildInfo = GetEntity("/guilds/{}", id.AsString()).Unwrap();
        mGuilds.insert_or_assign(id, Entity::Guild::Parse(guildInfo).Unwrap());
        return &*mGuilds.at(id);
    }


    const Entity::Guild* Bot::GetGuild(const Snowflake& id) const
    {
        if (mGuilds.contains(id))
        {
            return mGuilds.at(id).AsPtr().UnwrapOr(nullptr);
        }

        return nullptr;
    }


    std::unordered_set<Snowflake> Bot::FetchChannels(const Strawberry::Discord::Snowflake& guildId)
    {
        std::unordered_set<Snowflake> result;
        nlohmann::json                responseJSON = GetEntity("/guilds/{}/channels", guildId.AsString()).Unwrap();
        Core::Assert(responseJSON.is_array());
        // Add to lise of known guilds.
        for (const auto& channelJSON: responseJSON)
        {
            auto channel = Entity::Channel::Parse(channelJSON).Unwrap();
            result.insert(channel.GetId());

            // Cache guild id.
            if (!mChannels.contains(channel.GetId()))
            {
                mChannels.insert({channel.GetId(), channel});
            }
        }

        return result;
    }


    std::unordered_set<Snowflake> Bot::GetChannels(const Strawberry::Discord::Snowflake& guildId) const
    {
        std::unordered_set<Snowflake> result;
        result.reserve(mGuilds.size());

        for (auto& [id, data]: mChannels)
        {
            result.insert(id);
        }

        return std::move(result);
    }


    const Entity::Channel* Bot::FetchChannel(const Snowflake& id)
    {
        auto channelInfo = GetEntity("/channels/{}", id.AsString()).Unwrap();
        mChannels.insert_or_assign(id, Entity::Channel::Parse(channelInfo).Unwrap());
        return &*mChannels.at(id);
    }


    const Entity::Channel* Bot::GetChannel(const Snowflake& id) const
    {
        if (mChannels.contains(id))
        {
            return mChannels.at(id).AsPtr().UnwrapOr(nullptr);
        }
        else
        {
            return nullptr;
        }
    }


    void Bot::SendMessage(Snowflake channel, const std::string& message)
    {
        nlohmann::json json;
        json["content"] = message;

        PostRequest(json, "/channels/{}/messages", channel);
    }


    Core::Optional<nlohmann::json> Bot::PostRequest(const nlohmann::json& json, const std::string& endpoint)
    {
        static constexpr const char* API_PREFIX = "/api/v10";

        // Setup
        Core::Assert(endpoint.starts_with('/'));
        HTTP::Request request(HTTP::Verb::POST, fmt::format("{}{}", API_PREFIX, endpoint));
        request.GetHeader().Add("Authorization", fmt::format("Bot {}", mToken));
        request.GetHeader().Add("Host", "discord.com");
        request.GetHeader().Add("Content-Type", "application/json");
        std::string jsonString = json.dump(1, '\t');
        request.SetPayload(jsonString);
        request.GetHeader().Add("Content-Length", std::to_string(jsonString.length()));

        auto http = mHTTPS.Lock();
        http->SendRequest(request);

        HTTP::Response response = http->Receive();
        switch (response.GetStatus())
        {
            case 200: try
            {
                return nlohmann::json::parse(response.GetPayload().AsString());
            }
            catch (const std::exception& e)
            {
                Core::Unreachable();
            }

            case 401: return Core::NullOpt;

            case 429:
            {
                float                        waitTime = std::strtof(response.GetHeader().Get("X-RateLimit-Reset-After").c_str(), nullptr);
                std::chrono::duration<float> timeToWait(waitTime);
                std::this_thread::sleep_for(timeToWait);
                return GetEntity(endpoint);
            }

            default: Core::Logging::Error("{}", response.GetPayload().AsString());
            Core::Unreachable();
        }
    }


    Core::Optional<nlohmann::json> Bot::GetEntity(const std::string& endpoint)
    {
        using namespace Strawberry::Net;

        static constexpr const char* API_PREFIX = "/api/v10";

        // Setup
        Core::Assert(endpoint.starts_with('/'));
        HTTP::Request request(HTTP::Verb::GET, fmt::format("{}{}", API_PREFIX, endpoint));
        request.GetHeader().Add("Authorization", fmt::format("Bot {}", mToken));
        request.GetHeader().Add("Host", "discord.com");

        auto http = mHTTPS.Lock();
        http->SendRequest(request);

        HTTP::Response response = http->Receive();
        switch (response.GetStatus())
        {
            case 200: try
                {
                    return nlohmann::json::parse(response.GetPayload().AsString());
                }
                catch (const std::exception& e)
                {
                    Core::Unreachable();
                }

            case 401: return Core::NullOpt;

            case 429:
            {
                float                        waitTime = std::strtof(response.GetHeader().Get("X-RateLimit-Reset-After").c_str(), nullptr);
                std::chrono::duration<float> timeToWait(waitTime);
                std::this_thread::sleep_for(timeToWait);
                return GetEntity(endpoint);
            }

            default: Core::Logging::Error("{}", response.GetPayload().AsString());
                Core::Unreachable();
        }
    }


    Core::Optional<Net::Endpoint> Bot::GetGatewayEndpoint()
    {
        auto json = GetEntity("/gateway/bot");
        if (!json) return Core::NullOpt;
        auto url = static_cast<std::string>(json.Value()["url"]);
        url.erase(0, 6);
        return Endpoint::Resolve(url, 443).Unwrap();
    }
} // namespace Strawberry::Discord
