//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include "VoiceState.hpp"


//======================================================================================================================
//		Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Discord::Entity
{
	Core::Result<VoiceState, Error> VoiceState::Parse(const nlohmann::json& json) noexcept
	{
		VoiceState result;
		result.mGuildID   = Snowflake(std::string(json["guild_id"]));
		result.mChannelID = Snowflake(std::string(json["channel_id"]));
		result.mUserID = Snowflake(std::string(json["user_id"]));
		result.mGuildMember = GuildMember::Parse(json["member"]).Unwrap();
		return result;
	}
}
