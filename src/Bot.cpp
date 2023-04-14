#include <utility>

#include "Discord/Bot.hpp"
#include "Discord/Event/GuildCreate.hpp"



using namespace Strawberry::Core::Net;
using Strawberry::Core::Assert;
using Strawberry::Core::Unreachable;
using Strawberry::Core::Net::HTTP::HTTPSClient;




namespace Strawberry::Discord
{
	Bot::Bot(Token token, Intent intents)
			: mRunning(true)
			, mToken(std::move(token))
			, mIntents(intents)
			, mHTTPS("discord.com")
			, mGateway(GetGatewayEndpoint(), mToken, mIntents)
	{}



	void Bot::Run()
	{
		mRunning = true;
		while (mRunning)
		{
			auto gatewayMessage = mGateway.Lock()->Receive();
			if (gatewayMessage)
			{
				if (gatewayMessage->GetOpcode() == Core::Net::Websocket::Message::Opcode::Close)
				{
					std::cerr << "Websocket server closed with: " << gatewayMessage->GetCloseStatusCode() << std::endl;
				}

				OnGatewayMessage(gatewayMessage.Unwrap());
			}
			else
			{
				switch (gatewayMessage.Err())
				{
					case Websocket::Error::NoMessage:
						std::this_thread::yield();
						continue;
					case Websocket::Error::Closed:
						mRunning = false;
						break;
					case Websocket::Error::BadOp:
						continue;
					default:
						Unreachable();
				}
			}
		}
	}



	void Bot::Stop()
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
		mVoiceConnection.Emplace(mGateway, *mSessionId, guild, channel, *mUserId);
	}



	void Bot::DisconnectFromVoice()
	{
		mVoiceConnection.Reset();
	}


	std::unordered_set<Snowflake> Bot::FetchGuilds()
	{
		std::unordered_set<Snowflake> result;
		nlohmann::json responseJSON = GetEntity("/users/@me/guilds").Unwrap();
		Core::Assert(responseJSON.is_array());
		// Add to list of known guilds.
		for (auto guildJSON: responseJSON)
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

		for (auto& [id, data] : mGuilds)
		{
			result.insert(id);
		}

		return std::move(result);
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
		nlohmann::json responseJSON = GetEntity("/guilds/{}/channels", guildId.AsString()).Unwrap();
		Core::Assert(responseJSON.is_array());
		// Add to lise of known guilds.
		for (auto channelJSON : responseJSON)
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

		for (auto& [id, data] : mChannels)
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



	void Bot::RegisterEventListener(EventListener* listener)
	{
		listener->mRegistry = this->mEventListenerRegistry;
		mEventListenerRegistry.Lock()->insert(listener);
	}



	void Bot::DeregisterEventListener(EventListener* listener)
	{
		auto eventListeners = mEventListenerRegistry.Lock();
		if (eventListeners->contains(listener))
		{
			eventListeners->erase(listener);
			listener->mRegistry = nullptr;
		}
	}



	template<>
	Core::Option<nlohmann::json> Bot::GetEntity(const std::string& endpoint)
	{
		using namespace Strawberry::Core::Net;

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
			case 200:
				try
				{
					return nlohmann::json::parse(response.GetPayload().AsString());
				}
				catch (std::exception e)
				{
					return {};
				}
			case 429:
			{
				float waitTime = std::strtof(response.GetHeader().Get("X-RateLimit-Reset-After").c_str(), nullptr);
				std::chrono::duration<float> timeToWait(waitTime);
				std::this_thread::sleep_for(timeToWait);
				return GetEntity(endpoint);
			}

			default:
				Core::Unreachable();

		}

	}



	void Bot::OnGatewayMessage(const Websocket::Message& message)
	{
		auto json = message.AsJSON().UnwrapOr({});
		if (json.is_null())
		{
			return;
		}

		switch (static_cast<int>(json["op"]))
		{
			case 0: // Update Event
			{
				const std::string type = json["t"];
				if (type == "READY")
				{
					Event::Ready event = Event::Ready::Parse(json).Unwrap();
					mUserId			= event.GetUserId();
					mSessionId		= event.GetSessionId();
					if (mBehaviour) mBehaviour->OnReady(event);
					DispatchEvent(event);
				}
				else if (json["t"] == "GUILD_CREATE")
				{
					auto event = Event::GuildCreate::Parse(json).Unwrap();

					// Cache guilds and Channels
					mGuilds.insert_or_assign(event.GetGuild().GetId(), event.GetGuild());

					// Action event
					if (mBehaviour) mBehaviour->OnGuildCreate(event);
					DispatchEvent(event);
				}
				else if (json["t"] == "VOICE_SERVER_UPDATE")
				{
					if (mVoiceConnection)
					{
						mVoiceConnection->SetEndpoint(json["d"]["endpoint"]);
						mVoiceConnection->SetToken(json["d"]["token"]);
						if (mVoiceConnection->IsReady())
						{
							mVoiceConnection->Start();
						}
					}
				}
				else if (json["t"] == "VOICE_STATE_UPDATE")
				{
					// Print for later debugging purposes.
					std::cout << "Voice State Update" << std::endl;
					std::cout << json["d"].dump('\t') << std::endl;
				}
				else
				{
					std::cout << json.dump(1, '\t') << std::endl;
				}

				break;
			}

			case 4:


			case 11: // Heartbeat Acknowledge
				break;

			default:
				std::cout << std::setw(4) << json << std::endl;
				break;
		}
	}



	void Bot::DispatchEvent(const Event::EventBase& event) const
	{
		auto eventListeners = mEventListenerRegistry.Lock();
		for (auto listener : *eventListeners)
		{
			listener->ProcessEvent(event);
		}
	}



	std::string Bot::GetGatewayEndpoint()
	{
		auto json = GetEntity("/gateway/bot").Unwrap();
		auto url = static_cast<std::string>(json["url"]);
		url.erase(0, 6);
		return url;
	}
}