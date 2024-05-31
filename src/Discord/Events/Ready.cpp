#include "Discord/Events/Ready.hpp"


namespace Strawberry::Discord::Event
{
    Core::Result<Ready, Error> Strawberry::Discord::Event::Ready::Parse(const nlohmann::json& json)
    {
        Ready event;
        event.mUserId           = Snowflake(static_cast<std::string>(json["d"]["user"]["id"]));
        event.mSessionId        = static_cast<std::string>(json["d"]["session_id"]);
        event.mResumeGatewayURL = static_cast<std::string>(json["d"]["resume_gateway_url"]);
        return event;
    }


    Snowflake Strawberry::Discord::Event::Ready::GetUserId() const
    {
        return mUserId;
    }


    std::string Ready::GetSessionId() const
    {
        return mSessionId;
    }


    std::string Ready::GetResumeGatewayURL() const
    {
        return mResumeGatewayURL;
    }
} // namespace Strawberry::Discord::Event
