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


	const Snowflake& PermissionOverwrite::GetId() const
	{
		return mID;
	}


	int PermissionOverwrite::GetType() const
	{
		return mType;
	}


	uintmax_t PermissionOverwrite::GetAllowed() const
	{
		return mAllowed;
	}


	uintmax_t PermissionOverwrite::GetDenied() const
	{
		return mDenied;
	}
}