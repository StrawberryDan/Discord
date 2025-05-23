//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include "User.hpp"


//======================================================================================================================
//		Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Discord::Entity
{
	Core::Result<User, Error> User::Parse(const nlohmann::json& json)
	{
		User user;
		user.mID = Snowflake(std::string(json["id"]));
		return user;
	}
}
