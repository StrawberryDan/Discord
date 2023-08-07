#include "Discord/Entity/Guild.hpp"



namespace Strawberry::Discord::Entity
{
	Core::Result<Guild, Error> Guild::Parse(const nlohmann::json& json)
	{
		Guild guild;
		guild.mID = Snowflake(static_cast<std::string>(json["id"]));
		guild.mName = static_cast<std::string>(json["name"]);

		if (json.contains("permissions"))
			guild.mPermissions = std::stoull(std::string(json["permissions"]));

		return guild;
	}
}