#include <utility>

#include "Discord/Bot.hpp"
#include "Discord/Event/GuildCreate.hpp"



namespace Strawberry::Discord
{
	using Strawberry::Standard::Assert;
	using Strawberry::Standard::Unreachable;
	using Strawberry::Standard::Net::HTTP::HTTPSClient;



	Bot::Bot(Token token, Intent intents)
			: mRunning(true)
			, mToken(std::move(token))
			, mIntents(intents), mHTTPS("discord.com")
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
					case WSSClient::Error::NoMessage:
						std::this_thread::yield();
						continue;
					case WSSClient::Error::Closed:
						mRunning = false;
						break;
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



	void Bot::OnGatewayMessage(Message message)
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
					Event::Ready event;
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



	std::string Bot::GetGatewayEndpoint()
	{
		using Strawberry::Standard::Net::HTTP::Request;
		using Strawberry::Standard::Net::HTTP::Verb;
		using Strawberry::Standard::Net::HTTP::ChunkedPayload;



		Request request(Verb::GET, "/api/v10/gateway/bot");
		request.GetHeader().Add("Authorization", fmt::format("Bot {}", mToken));
		request.GetHeader().Add("Host", "discord.com");
		mHTTPS.Lock()->SendRequest(request);
		auto response = mHTTPS.Lock()->Receive();
		Assert(response.GetStatus() == 200);
		auto payload = std::get<ChunkedPayload>(*response.GetPayload());
		auto json = *payload[0].AsJSON();
		auto url = (std::string) json["url"];
		url.erase(0, 6);
		return url;
	}
}