#include "Discord/Entity/Guild.hpp"



namespace Strawberry::Discord::Entity
{
	Core::Result<Guild, Error> Guild::Parse(const nlohmann::json& json)
	{
		Guild guild;
		guild.mGuildId = Snowflake(static_cast<std::string>(json["id"]));
		guild.mGuildName = static_cast<std::string>(json["name"]);

		for (auto channelInfo : json["channels"])
		{
			auto channel = Channel::Parse(channelInfo).Unwrap();
			channel.mGuildId = guild.mGuildId;
			guild.mChannels.emplace_back(std::move(channel));
		}

		return guild;
	}
}