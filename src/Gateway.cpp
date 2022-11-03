#include "Discord/Gateway.hpp"



namespace
{
	using Strawberry::Standard::Assert;
}



namespace Strawberry::Discord
{
	Gateway::Gateway(std::string endpoint, std::string token, Intent intent)
		: mWSS(WSSClient::Connect(endpoint, "/?v=10&encoding=json").Unwrap())
		, mHeartbeat()
	{
		auto hello = Receive().Unwrap().AsJSON().Unwrap();
		Assert(hello["op"] == 10);
		mHeartbeat.Emplace(mWSS, static_cast<double>(hello["d"]["heartbeat_interval"]) / 1000.0);

		nlohmann::json identifier;
		identifier["op"]                         = 2;
		identifier["d"]["token"]                 = token;
		identifier["d"]["intents"]               = std::to_underlying(intent);
		identifier["d"]["properties"]["os"]      = "windows";
		identifier["d"]["properties"]["browser"] = "strawberry";
		identifier["d"]["properties"]["device"]  = "strawberry";
		Send(Message(to_string(identifier)));
	}



	Result<Message, WSSClient::Error> Gateway::Receive()
	{
		auto msg = mWSS.Lock()->ReadMessage();

		if (msg)
		{
			auto json = msg->AsJSON();
			if (mHeartbeat && json && json->at("s").is_number())
			{
				mHeartbeat->UpdateSequenceNumber(json.Unwrap()["s"]);
			}

			return msg;
		}
		else
		{
			switch (msg.Err())
			{
				case WSSClient::Error::NoMessage:
				case WSSClient::Error::Closed:
					return msg.Err();
				default:
					Standard::Unreachable();
			}
		}
	}



	void Gateway::Send(const Message& msg)
	{
		mWSS.Lock()->SendMessage(msg);
	}
}