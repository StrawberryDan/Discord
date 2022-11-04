#include "Discord/Voice/Connection.hpp"



#include "nlohmann/json.hpp"



Strawberry::Discord::Voice::Connection::Connection(std::string endpoint,
												   std::string sessionId,
												   std::string token,
												   Snowflake guildId,
												   Snowflake userId)
	: mWSS(Standard::Net::Websocket::WSSClient::Connect(endpoint, "/?v=4").Unwrap())
{
	using nlohmann::json;
	using Standard::Net::Websocket::Message;


	json ident;
	ident["op"] = 0;
	ident["d"]["server_id"] = guildId.AsString();
	ident["d"]["user_id"] = userId.AsString();
	ident["d"]["session_id"] = sessionId;
	ident["d"]["token"] = token;
	mWSS.Lock()->SendMessage(Message(ident.dump()));

	auto hello = mWSS.Lock()->WaitMessage().Unwrap().AsJSON().Unwrap();
	auto heartbeatInterval = static_cast<double>(hello["d"]["heartbeat_interval"]) / 1000.0;
	mHeartbeat.Emplace(mWSS, heartbeatInterval);
}