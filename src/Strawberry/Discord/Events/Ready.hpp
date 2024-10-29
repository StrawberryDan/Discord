#pragma once


#include "Strawberry/Discord/Error.hpp"
#include "Strawberry/Discord/Snowflake.hpp"
#include "Strawberry/Core/Types/Result.hpp"
#include "nlohmann/json.hpp"


namespace Strawberry::Discord::Event
{
    class Ready
    {
        public:
            static Core::Result<Ready, Error> Parse(const nlohmann::json& json);

        public:
            [[nodiscard]] Snowflake   GetUserId() const;
            [[nodiscard]] std::string GetSessionId() const;
            [[nodiscard]] std::string GetResumeGatewayURL() const;

        private:
            Snowflake   mUserId;
            std::string mSessionId;
            std::string mResumeGatewayURL;
    };
} // namespace Strawberry::Discord::Event
