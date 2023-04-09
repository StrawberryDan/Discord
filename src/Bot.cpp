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



	void Bot::ConnectToVoice(Snowflake guild, Snowflake channel)
	{
		RequestVoiceInfo(guild, channel);
		while (!mVoiceEndpoint || !mVoiceSessionId || !mVoiceToken || !mUserId)
		{
			std::this_thread::yield();
		}

		mVoiceConnection.Emplace(*mVoiceEndpoint, *mVoiceSessionId, *mVoiceToken, guild, *mUserId);
	}



	const Entity::Channel* Bot::GetChannelById(const Snowflake& id) const
	{
		if (mChannels.contains(id))
		{
			return &mChannels.at(id);
		}
		else
		{
			return nullptr;
		}
	}



	void Bot::SetBehaviour(std::unique_ptr<Behaviour> behaviour)
	{
		mBehaviour = std::move(behaviour);
	}



	void Bot::RegisterEventListener(EventListener* listener)
	{
		mEventListeners.insert(listener);
	}



	void Bot::DeregisterEventListener(EventListener* listener)
	{
		Assert(mEventListeners.contains(listener));
		mEventListeners.erase(listener);
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
					mUserId = event.GetUserId();
					if (mBehaviour) mBehaviour->OnReady(event);
					DispatchEvent(event);
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
					mVoiceEndpoint = json["d"]["endpoint"];
					mVoiceEndpoint->erase(mVoiceEndpoint->find(":"), mVoiceEndpoint->size());
					mVoiceToken    = json["d"]["token"];
				}
				else if (json["t"] == "VOICE_STATE_UPDATE")
				{
					mVoiceSessionId = json["d"]["session_id"];
				}
				else
				{
					std::cout << json.dump(1, '\t') << std::endl;
				}

				break;
			}

			case 11: // Heartbeat Acknowledge
				break;

			default:
				std::cout << std::setw(4) << json << std::endl;
				break;
		}
	}



	void Bot::DispatchEvent(const Event::Base& event) const
	{
		for (auto listener : mEventListeners)
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
			request["d"]["self_deaf"] = true;


			mVoiceEndpoint.Reset();
			mVoiceToken.Reset();
			mVoiceSessionId.Reset();
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