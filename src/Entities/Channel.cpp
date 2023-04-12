#include "Discord/Entity/Channel.hpp"



namespace Strawberry::Discord::Entity
{
	Core::Result<Channel, Error> Channel::Parse(const nlohmann::json& json)
	{
		Channel channel;
		channel.mId			= Snowflake(static_cast<std::string>(json["id"]));
		channel.mName		= static_cast<std::string>(json["name"]);
		channel.mType		= static_cast<Type>(json["type"]);
		channel.mPosition	= static_cast<size_t>(json["position"]);

		if (json.contains("guild_id"))
		{
			channel.mGuildId.Emplace(static_cast<std::string>(json["guild_id"]));
		}

		return channel;
	}
}
