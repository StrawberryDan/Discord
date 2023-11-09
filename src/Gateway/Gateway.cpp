#include "Discord/Gateway/Gateway.hpp"


namespace Strawberry::Discord::Gateway
{
	Core::Optional<Gateway>
	Gateway::Connect(const Net::Endpoint& endpoint, const std::string& token, Intent intents)
	{
		Gateway gateway(endpoint, token, intents);

		if (!gateway.IsOk()) return Core::NullOpt;

		return gateway;
	}

	bool Gateway::IsOk() const
	{
		return mWSS && mHeartbeat;
	}


	Gateway::Gateway(const Net::Endpoint& endpoint, const std::string& token, Intent intent)
		: mWSS(Net::Websocket::WSSClient::Connect(endpoint, "/?v=10&encoding=json").IntoOptional().Map([](Net::Websocket::WSSClient&& x) {
			return Core::SharedMutex(std::move(x));
		}))
		, mHeartbeat()
	{
		auto helloMessage = Receive();
		while (!helloMessage && helloMessage.Err() == Net::Websocket::Error::NoMessage)
		{
			std::this_thread::yield();
			helloMessage = Receive();
		}

		auto helloJson = helloMessage.Unwrap().AsJSON().Unwrap();
		Core::Assert(helloJson["op"] == 10);

		if (mWSS)
		{
			mHeartbeat.reset(new Heartbeat(*mWSS, static_cast<double>(helloJson["d"]["heartbeat_interval"]) / 1000.0));

			nlohmann::json identifier;
			identifier["op"]                         = 2;
			identifier["d"]["token"]                 = token;
			identifier["d"]["intents"]               = std::to_underlying(intent);
			identifier["d"]["properties"]["os"]      = "windows";
			identifier["d"]["properties"]["browser"] = "strawberry";
			identifier["d"]["properties"]["device"]  = "strawberry";
			Send(Net::Websocket::Message(identifier)).Unwrap();
		}
	}


	Gateway::ReceiveResult Gateway::Receive(bool checkBuffer)
	{
		// Early return if we can just take a message from the buffer.
		if (checkBuffer && BufferedMessageCount() > 0)
		{
			auto message = std::move(mMessageBuffer.front());
			mMessageBuffer.pop();
			return message;
		}

		auto msg = mWSS->Lock()->ReadMessage();

		if (msg)
		{
			auto json = msg->AsJSON();
			if (mHeartbeat && json && json->at("s").is_number()) { mHeartbeat->UpdateSequenceNumber(json.Unwrap()["s"]); }

			return msg;
		}
		else
		{
			switch (msg.Err())
			{
				case Net::Websocket::Error::NoMessage:
				case Net::Websocket::Error::Closed:
				case Net::Websocket::Error::ProtocolError:
					return msg.Err();
				default:
					Core::Unreachable();
			}
		}
	}


	Gateway::SendResult Gateway::Send(const Net::Websocket::Message& msg)
	{
		return mWSS->Lock()->SendMessage(msg);
	}


	size_t Gateway::BufferedMessageCount()
	{
		return mMessageBuffer.size();
	}


	void Gateway::BufferMessage(Net::Websocket::Message message)
	{
		mMessageBuffer.emplace(std::move(message));
	}
} // namespace Strawberry::Discord::Gateway
