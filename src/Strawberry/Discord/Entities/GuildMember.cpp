//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include "GuildMember.hpp"


//======================================================================================================================
//		Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Discord::Entity
{
	Core::Result<GuildMember, Error> GuildMember::Parse(const nlohmann::json& json) noexcept
	{
		GuildMember guildMember;

		auto userParse = User::Parse(json["user"]);
		if (!userParse) return userParse.Err();
		guildMember.mUser = userParse.Unwrap();

		return guildMember;
	}
}