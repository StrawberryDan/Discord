#include <utility>

#include "Discord/Bot.hpp"
#include "Discord/Event/GuildCreate.hpp"



namespace Strawberry::Discord
{
	using namespace Strawberry::Standard::Net;



	Bot::Bot(Token token, Intent intents)
			: mRunning(true), mToken(std::move(token)), mIntents(intents), mHTTPS("discord.com"),
			  mGateway(GetGatewayEndpoint(), mToken, mIntents)
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
				continue;
				Unreachable();;
			}
		}
	}



	void Bot::SetBehaviour(std::unique_ptr<Behaviour> behaviour)
	{
		mBehaviour = std::move(behaviour);
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
				}
				else if (json["t"] == "GUILD_CREATE")
				{
					auto event = Event::GuildCreate::Parse(json).Unwrap();
					if (mBehaviour) mBehaviour->OnGuildCreate(event);
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



	std::string Bot::GetGatewayEndpoint()
	{
		HTTP::Request request(HTTP::Verb::GET, "/api/v10/gateway/bot");
		request.GetHeader().Add("Authorization", fmt::format("Discord {}", mToken));
		request.GetHeader().Add("Host", "discord.com");
		mHTTPS.Lock()->SendRequest(request);
		auto response = mHTTPS.Lock()->Receive();
		Assert(response.GetStatus() == 200);
		auto payload = std::get<HTTP::ChunkedPayload>(*response.GetPayload());
		auto json = *payload[0].AsJSON();
		auto url = (std::string) json["url"];
		url.erase(0, 6);
		return url;
	}
}