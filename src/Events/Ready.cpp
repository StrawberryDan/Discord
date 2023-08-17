#include "Discord/Event/Ready.hpp"


namespace Strawberry::Discord::Event
{
	Core::Result<Ready, Error> Strawberry::Discord::Event::Ready::Parse(const nlohmann::json& json)
	{
		Ready event;
		event.mUserId    = Snowflake(static_cast<std::string>(json["d"]["user"]["id"]));
		event.mSessionId = static_cast<std::string>(json["d"]["session_id"]);
		return event;
	}


	Snowflake Strawberry::Discord::Event::Ready::GetUserId() const { return mUserId; }


	std::string Ready::GetSessionId() const { return mSessionId; }
} // namespace Strawberry::Discord::Event