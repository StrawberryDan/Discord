#include "Discord/Event/GuildCreate.hpp"



#include "Standard/Assert.hpp"



namespace Strawberry::Discord::Event
{
	Result<GuildCreate, Error> GuildCreate::Parse(const nlohmann::json& json)
	{
		if(json["op"] != 0 || json["t"] != "GUILD_CREATE")
		{
			return Error(Error::Type::InvalidJSON);
		}

		auto info = json["d"];

		GuildCreate event;
		event.mGuild = Entity::Guild::Parse(info).Unwrap();

		return event;
	}
}


