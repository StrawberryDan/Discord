#include "Discord/Voice/Connection.hpp"



#include "nlohmann/json.hpp"
#include "Core/Net/Address.hpp"
#include "Core/Net/Endpoint.hpp"



Strawberry::Discord::Voice::Connection::Connection(std::string endpoint,
												   std::string sessionId,
												   std::string token,
												   Snowflake guildId,
												   Snowflake userId)
	: mWSS(Core::Net::Websocket::WSSClient::Connect(endpoint, "/?v=4").Unwrap())
{
	using nlohmann::json;
	using Core::Net::Websocket::Message;
	auto wss = mWSS.Lock();


	json ident;
	ident["op"] = 0;
	ident["d"]["server_id"] = guildId.AsString();
	ident["d"]["user_id"] = userId.AsString();
	ident["d"]["session_id"] = sessionId;
	ident["d"]["token"] = token;
	wss->SendMessage(Message(ident.dump())).Unwrap();


	// Receive Hello
	auto helloMsg  = wss->WaitMessage().Unwrap();
	auto helloJSON = helloMsg.AsJSON().Unwrap();
	auto heartbeatInterval = static_cast<double>(helloJSON["d"]["heartbeat_interval"]) / 1000.0;
	mHeartbeat.Emplace(mWSS, heartbeatInterval);


	// Receive Voice Ready
	auto readyMsg  = wss->WaitMessage().Unwrap();
	auto readyJSON = readyMsg.AsJSON().Unwrap();
	std::cerr << readyJSON.dump('\t') << std::endl;
	Core::Assert(readyJSON["op"] == 2);
	auto addr = Strawberry::Core::Net::IPv4Address::Parse(readyJSON["d"]["ip"]).Unwrap();
	uint16_t port = readyJSON["d"]["port"];
	std::vector<std::string> modes = readyJSON["d"]["modes"];
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
	auto sessionDescriptionMsg = wss->WaitMessage().Unwrap();
	auto sessionDescriptionJSON = sessionDescriptionMsg.AsJSON().Unwrap();
	Core::Assert(sessionDescriptionJSON["op"] == 4);
	Core::Assert(sessionDescriptionJSON["d"]["mode"] == "xsalsa20_poly1305");
	mKey = sessionDescriptionJSON["d"]["secret_key"];
}