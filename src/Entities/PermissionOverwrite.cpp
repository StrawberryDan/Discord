//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Discord/Entity/PermissionOverwrite.hpp"


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Discord::Entity
{
	PermissionOverwrite::PermissionOverwrite(Discord::Snowflake id, int type, std::uintmax_t allow, std::uintmax_t deny)
		: mID(id)
		, mType(type)
		, mAllowed(allow)
		, mDenied(deny)
	{

	}
}