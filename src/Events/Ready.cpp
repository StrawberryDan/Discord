#include "Discord/Event/Ready.hpp"



namespace Strawberry::Discord::Event
{
	Core::Result<Ready, Error> Strawberry::Discord::Event::Ready::Parse(const nlohmann::json& json)
	{
		Ready event;
		event.mUserId = Snowflake(static_cast<std::string>(json["d"]["user"]["id"]));
		return event;
	}



	Snowflake Strawberry::Discord::Event::Ready::GetUserId() const
	{
		return mUserId;
	}
}