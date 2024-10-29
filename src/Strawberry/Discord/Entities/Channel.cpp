#include "Strawberry/Discord/Entities/Channel.hpp"


namespace Strawberry::Discord::Entity
{
    Core::Result<Channel, Error> Channel::Parse(const nlohmann::json& json)
    {
        Channel channel;
        channel.mId       = Snowflake(static_cast<std::string>(json["id"]));
        channel.mName     = static_cast<std::string>(json["name"]);
        channel.mType     = static_cast<Type>(json["type"]);
        channel.mPosition = static_cast<size_t>(json["position"]);

        if (json.contains("guild_id"))
        {
            channel.mGuildId.Emplace(static_cast<std::string>(json["guild_id"]));
        }

        if (json.contains("permission_overwrites"))
        {
            for (const auto& overwriteJSON: json["permission_overwrites"])
            {
                std::uintmax_t      allowBits(std::stoull(std::string(overwriteJSON["allow"])));
                std::uintmax_t      denyBits(std::stoull(std::string(overwriteJSON["deny"])));
                PermissionOverwrite overwrite(Snowflake(std::string(overwriteJSON.at("id"))),
                                              overwriteJSON.at("type"),
                                              allowBits,
                                              denyBits);
                channel.mPermissionOverwrites.emplace_back(overwrite);
            }
        }

        return channel;
    }
} // namespace Strawberry::Discord::Entity
