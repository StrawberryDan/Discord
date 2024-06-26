#pragma once


#include "Discord/Error.hpp"
#include "Discord/Snowflake.hpp"
#include "EventBase.hpp"
#include "Strawberry/Core/Types/Result.hpp"
#include "nlohmann/json.hpp"


namespace Strawberry::Discord::Event
{
    class Ready : public EventBase
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
