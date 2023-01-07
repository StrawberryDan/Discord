#include "Discord/Voice/Connection.hpp"



#include "nlohmann/json.hpp"
#include "Standard/Net/Address.hpp"
#include "Standard/Net/Endpoint.hpp"



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


	// Receive Hello
	auto hello = mWSS.Lock()->WaitMessage().Unwrap().AsJSON().Unwrap();
	auto heartbeatInterval = static_cast<double>(hello["d"]["heartbeat_interval"]) / 1000.0;
	mHeartbeat.Emplace(mWSS, heartbeatInterval);


	// Receive Voice Ready
	auto ready = mWSS.Lock()->WaitMessage().Unwrap().AsJSON().Unwrap();
	Standard::Assert(ready["op"] == 2);
	auto addr = Strawberry::Standard::Net::IPv4Address::Parse(ready["d"]["ip"]).Unwrap();
	uint16_t port = ready["d"]["port"];
	std::vector<std::string> modes = ready["d"]["modes"];
	Standard::Assert(std::find(modes.begin(), modes.end(), "xsalsa20_poly1305") != modes.end());
	mUDP = Standard::Net::Socket::UDPClient::Create().Unwrap();


	// Send protocol selection
	nlohmann::json protocolSelect;
	protocolSelect["op"] = 1;
	protocolSelect["d"]["protocol"] = "udp";
	protocolSelect["d"]["data"]["address"] = addr.AsString();
	protocolSelect["d"]["data"]["port"] = port;
	protocolSelect["d"]["data"]["mode"] = "xsalsa20_poly1305";
	mWSS.Lock()->SendMessage(Standard::Net::Websocket::Message(protocolSelect.dump()));


	// Receive session description
	auto sessionDescription = mWSS.Lock()->WaitMessage().Unwrap().AsJSON().Unwrap();
	Standard::Assert(sessionDescription["op"] == 4);
	Standard::Assert(sessionDescription["d"]["mode"] == "xsalsa20_poly1305");
	mKey = sessionDescription["d"]["secret_key"];
}