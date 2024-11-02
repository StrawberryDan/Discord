//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include "MessageCreate.hpp"
#include "Strawberry/Core/IO/Error.hpp"


//======================================================================================================================
//		Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Discord
{
	Core::Result<Event::MessageCreate, Error> Event::MessageCreate::Parse(const nlohmann::json& json)
	{
		Core::AssertEQ(json["t"], "MESSAGE_CREATE");

		MessageCreate event;
		event.mContents = json["d"]["content"];
		event.mChannel = Snowflake(std::string(json["d"]["channel_id"]));
		return event;
	}
}
