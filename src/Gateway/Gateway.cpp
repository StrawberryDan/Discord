#include "Discord/Gateway/Gateway.hpp"



namespace Strawberry::Discord::Gateway
{
	Gateway::Gateway(std::string endpoint, std::string token, Intent intent)
		: mWSS(Core::Net::Websocket::WSSClient::Connect(endpoint, "/?v=10&encoding=json").Unwrap())
		, mHeartbeat()
	{
		auto hello = Receive().Unwrap().AsJSON().Unwrap();
		Core::Assert(hello["op"] == 10);
		mHeartbeat.Emplace(mWSS, static_cast<double>(hello["d"]["heartbeat_interval"]) / 1000.0);

		nlohmann::json identifier;
		identifier["op"]                         = 2;
		identifier["d"]["token"]                 = token;
		identifier["d"]["intents"]               = std::to_underlying(intent);
		identifier["d"]["properties"]["os"]      = "windows";
		identifier["d"]["properties"]["browser"] = "strawberry";
		identifier["d"]["properties"]["device"]  = "strawberry";
		Send(Core::Net::Websocket::Message(identifier.dump()));
	}



	Core::Result<Core::Net::Websocket::Message, Core::Net::Websocket::Error> Gateway::Receive()
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
				case Core::Net::Websocket::Error::NoMessage:
				case Core::Net::Websocket::Error::Closed:
				case Core::Net::Websocket::Error::BadOp:
					return msg.Err();
				default:
					Core::Unreachable();
			}
		}
	}



	void Gateway::Send(const Core::Net::Websocket::Message& msg)
	{
		mWSS.Lock()->SendMessage(msg);
	}
}