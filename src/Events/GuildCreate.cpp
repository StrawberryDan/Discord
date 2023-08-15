#include "Discord/Event/GuildCreate.hpp"


namespace Strawberry::Discord::Event
{
	Core::Result<GuildCreate, Error> GuildCreate::Parse(const nlohmann::json& json)
	{
		if (json["op"] != 0 || json["t"] != "GUILD_CREATE")
		{
			return Error(Error::Type::InvalidJSON);
		}

		const auto& info = json["d"];

		GuildCreate event;
		event.mGuild = Entity::Guild::Parse(info).Unwrap();

		return event;
	}
}// namespace Strawberry::Discord::Event


const Strawberry::Discord::Entity::Guild& Strawberry::Discord::Event::GuildCreate::GetGuild() const
{
	return mGuild;
}
