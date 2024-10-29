#pragma once


#include "Channel.hpp"
#include "Strawberry/Discord/Error.hpp"
#include "Strawberry/Discord/Snowflake.hpp"
#include "Strawberry/Core/Types/Variant.hpp"
#include "nlohmann/json.hpp"

#include <string>
#include <unordered_set>


namespace Strawberry::Discord::Entity
{
    class Guild
    {
        public:
            static Core::Result<Guild, Error> Parse(const nlohmann::json& json);

        public:
            [[nodiscard]] inline const Snowflake& GetId() const
            {
                return mID;
            }


            [[nodiscard]] inline const std::string& GetName() const
            {
                return mName;
            }


            [[nodiscard]] inline Core::Optional<std::uintmax_t> GetPermissions() const
            {
                return mPermissions;
            }

        private:
            Snowflake                      mID;
            std::string                    mName;
            Core::Optional<std::uintmax_t> mPermissions;
    };
} // namespace Strawberry::Discord::Entity
