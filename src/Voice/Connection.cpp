#include "Discord/Voice/Connection.hpp"



#include "nlohmann/json.hpp"
#include "Strawberry/Core/Net/Address.hpp"
#include "Strawberry/Core/Net/Endpoint.hpp"



namespace Strawberry::Discord::Voice
{
	Connection::Connection(Core::SharedMutex<Gateway::Gateway> gateway,
						   std::string sessionId,
						   Snowflake guildId,
						   Snowflake channelId,
						   Snowflake userId)
		: mGateway(gateway)
		, mWSS(nullptr)
		, mGuild(guildId)
		, mChannel(channelId)
		, mUser(userId)
		, mSessionId(sessionId)
	{
		using nlohmann::json;
		using namespace Core::Net::Websocket;

		auto gatewayLock = mGateway.Lock();

		json request;
		request["op"]				= 4;
		request["d"]["guild_id"]	= mGuild.AsString();
		request["d"]["channel_id"]	= mChannel.AsString();
		request["d"]["self_mute"]	= false;
		request["d"]["self_deaf"]	= false;

		Message msg(request.dump());
		gatewayLock->Send(msg).Unwrap();

		while (true)
		{
			auto message = gatewayLock->Receive(false).Unwrap();
			auto messageAsJsonResult = message.AsJSON();
			if (!messageAsJsonResult)
			{
				gatewayLock->BufferMessage(std::move(message));
			}

			auto json = messageAsJsonResult.Unwrap();
			if (json["t"] == "VOICE_SERVER_UPDATE")
			{
				SetEndpoint(json["d"]["endpoint"]);
				SetToken(json["d"]["token"]);
				Core::Assert(IsReady());
				Start();
				break;
			}
			else
			{
				gatewayLock->BufferMessage(std::move(message));
			}
		}
	}



	Connection::~Connection()
	{
		using nlohmann::json;
		using namespace Core::Net::Websocket;

		json request;
		request["op"]				= 4;
		request["d"]["guild_id"]	= mGuild.AsString();
		request["d"]["channel_id"]	= {};
		request["d"]["self_mute"]	= false;
		request["d"]["self_deaf"]	= false;

		Message msg(request.dump());
		mGateway.Lock()->Send(msg).Unwrap();
	}



	void Connection::SetEndpoint(std::string endpoint)
	{
        // Erase port number
		endpoint.erase(endpoint.find(":"), endpoint.size());
		mEndpoint = endpoint;
	}



	void Connection::SetToken(std::string token)
	{
		mToken = token;
	}



	void Connection::SetSessionId(std::string id)
	{
		mSessionId = id;
	}



	bool Connection::IsReady() const
	{
		return mEndpoint && mToken;
	}



	void Connection::Start()
	{
		using nlohmann::json;
		using namespace Core::Net::Websocket;

		// Start websocket
		mWSS.Emplace(WSSClient::Connect(*mEndpoint, "/?v=4").Unwrap());

		// Identify with the voice server
		json ident;
		ident["op"]					= 0;
		ident["d"]["server_id"]		= mGuild.AsString();
		ident["d"]["user_id"]		= mUser.AsString();
		ident["d"]["session_id"]	= mSessionId;
		ident["d"]["token"]			= *mToken;
		auto wss = mWSS.Lock();
		wss->SendMessage(Message(ident.dump())).Unwrap();

		// Receive Hello
		auto hello = wss->WaitMessage().Unwrap().AsJSON().Unwrap();
		auto heartbeatInterval = static_cast<double>(hello["d"]["heartbeat_interval"]) / 1000.0;
		mHeartbeat.Emplace(mWSS, heartbeatInterval);

		// Receive Voice Ready
		auto ready = wss->WaitMessage().Unwrap().AsJSON().Unwrap();
		Core::Assert(ready["op"] == 2);
		auto addr = Strawberry::Core::Net::IPv4Address::Parse(ready["d"]["ip"]).Unwrap();
		uint16_t port = ready["d"]["port"];
		std::vector<std::string> modes = ready["d"]["modes"];
		Core::Assert(std::find(modes.begin(), modes.end(), "xsalsa20_poly1305") != modes.end());
		mUDP = Core::Net::Socket::UDPClient::Create().Unwrap();

		// Send protocol selection
		nlohmann::json protocolSelect;
		protocolSelect["op"] = 1;
		protocolSelect["d"]["protocol"] = "udp";
		protocolSelect["d"]["data"]["address"] = addr.AsString();
		protocolSelect["d"]["data"]["port"] = port;
		protocolSelect["d"]["data"]["mode"] = "xsalsa20_poly1305";
		wss->SendMessage(Core::Net::Websocket::Message(protocolSelect.dump())).Unwrap();

		// Receive session description
		auto sessionDescription = wss->WaitMessage().Unwrap().AsJSON().Unwrap();
		Core::Assert(sessionDescription["op"] == 4);
		Core::Assert(sessionDescription["d"]["mode"] == "xsalsa20_poly1305");
		mKey = sessionDescription["d"]["secret_key"];
	}
}



