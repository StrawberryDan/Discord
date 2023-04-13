#include "Discord/Entity/Guild.hpp"



namespace Strawberry::Discord::Entity
{
	Core::Result<Guild, Error> Guild::Parse(const nlohmann::json& json)
	{
		Guild guild;
		guild.mGuildId = Snowflake(static_cast<std::string>(json["id"]));
		guild.mGuildName = static_cast<std::string>(json["name"]);
		return guild;
	}
}