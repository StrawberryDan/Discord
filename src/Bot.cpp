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
			auto gatewayMessage = mGateway->Receive();
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
		RequestVoiceInfo(guild, channel);
		while (!mVoiceEndpoint || !mVoiceSessionId || !mVoiceToken || !mUserId)
		{
			std::this_thread::yield();
		}

		mVoiceConnection.Emplace(*mVoiceEndpoint, *mVoiceSessionId, *mVoiceToken, guild, *mUserId);
	}



	std::unordered_set<Snowflake> Bot::FetchGuilds()
	{
		using namespace Strawberry::Core::Net;

		std::unordered_set<Snowflake> result;
		// Request guilds
		HTTP::Request request(HTTP::Verb::GET, "/api/v10/users/@me/guilds");
		request.GetHeader().Add("Authorization", fmt::format("Bot {}", mToken));
		request.GetHeader().Add("Host", "discord.com");
		// Receive Response
		auto https = mHTTPS.Lock();
		https->SendRequest(request);
		HTTP::Response response = https->Receive();
		Core::Assert(response.GetStatus() == 200);
		nlohmann::json responseJSON = nlohmann::json::parse(response.GetPayload().AsString());
		Core::Assert(responseJSON.is_array());
		// Add to lise of known guilds.
		for (auto guildJSON: responseJSON)
		{
			auto guild = Entity::Guild::Parse(guildJSON).Unwrap();
			result.insert(guild.GetId());

			// Cache guild id.
			if (!mGuilds.contains(guild.GetId()))
			{
				mGuilds.insert({guild.GetId(), {}});
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



	const Entity::Channel* Bot::GetChannelById(const Snowflake& id) const
	{
		if (mChannels.contains(id))
		{
			Core::Option<Entity::Channel> channel = mChannels.at(id);
			if (channel)
			{
				return &*mChannels.at(id);
			}
			else
			{
				// TODO: implement fetching from server here.
				return nullptr;
			}
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



	void Bot::OnGatewayMessage(Websocket::Message message)
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
					mVoiceSessionId = event.GetSessionId();
					if (mBehaviour) mBehaviour->OnReady(event);
					DispatchEvent(event);

					// Get the list of guilds we are in after we are ready.
					for (auto snowflake : FetchGuilds())
					{
						if (!mGuilds.contains(snowflake))
						{
							mGuilds.insert({snowflake, {}});
						}
					}
				}
				else if (json["t"] == "GUILD_CREATE")
				{
					auto event = Event::GuildCreate::Parse(json).Unwrap();

					// Cache guilds and Channels
					mGuilds.insert_or_assign(event.GetGuild().GetId(), event.GetGuild());
					for (const auto& channel : event.GetGuild().GetChannels())
					{
						mChannels.insert_or_assign(channel.GetId(), channel);
					}

					// Action event
					if (mBehaviour) mBehaviour->OnGuildCreate(event);
					DispatchEvent(event);
				}
				else if (json["t"] == "VOICE_SERVER_UPDATE")
				{
					std::cout << "Voice Server Update Received" << std::endl;
					mVoiceEndpoint = json["d"]["endpoint"];
					mVoiceEndpoint->erase(mVoiceEndpoint->find(":"), mVoiceEndpoint->size());
					mVoiceToken    = json["d"]["token"];
				}
				else if (json["t"] == "VOICE_STATE_UPDATE")
				{
					std::cout << "Voice State Update Received" << std::endl;
					mVoiceSessionId = json["d"]["session_id"];
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



	void Bot::RequestVoiceInfo(Snowflake guild, Snowflake channel)
	{
		using nlohmann::json;
		namespace WS = Core::Net::Websocket;

		if (guild != mVoiceGuild || channel != mVoiceChannel)
		{
			json request;
			request["op"] = 4;
			request["d"]["guild_id"] = guild.AsString();
			request["d"]["channel_id"] = channel.AsString();
			request["d"]["self_mute"] = false;
			request["d"]["self_deaf"] = false;


			mVoiceEndpoint.Reset();
			mVoiceToken.Reset();
			mVoiceGuild     = guild;
			mVoiceChannel   = channel;


			WS::Message msg(request.dump());
			mGateway->Send(msg).Unwrap();
		}
	}



	std::string Bot::GetGatewayEndpoint()
	{
		using Strawberry::Core::Net::HTTP::Request;
		using Strawberry::Core::Net::HTTP::Verb;



		Request request(Verb::GET, "/api/v10/gateway/bot");
		request.GetHeader().Add("Authorization", fmt::format("Bot {}", mToken));
		request.GetHeader().Add("Host", "discord.com");
		mHTTPS.Lock()->SendRequest(request);
		auto response = mHTTPS.Lock()->Receive();
		Assert(response.GetStatus() == 200);
		auto payload = response.GetPayload();
		auto string = payload.AsString();
		auto json = nlohmann::json::parse(string);
		auto url = static_cast<std::string>(json["url"]);
		url.erase(0, 6);
		return url;
	}
}