#pragma once


#include "Strawberry/Discord/Entities/Guild.hpp"
#include "Strawberry/Discord/Error.hpp"
#include <nlohmann/json.hpp>


namespace Strawberry::Discord::Event
{
    class GuildCreate
    {
        public:
            // Constructors
            static Core::Result<GuildCreate, Error> Parse(const nlohmann::json& json);


            [[nodiscard]] const Entity::Guild& GetGuild() const;

        private:
            GuildCreate() = default;

        private:
            Entity::Guild mGuild;
    };
} // namespace Strawberry::Discord::Event
