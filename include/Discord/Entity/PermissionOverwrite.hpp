#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Discord
#include "Discord/Snowflake.hpp"
// Standard Library
#include <cstdint>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Discord::Entity
{
	class PermissionOverwrite
	{
	public:
		PermissionOverwrite(Discord::Snowflake id, int type, std::uintmax_t allow, std::uintmax_t deny);


		const Snowflake& GetId() const;
		int              GetType() const;
		uintmax_t        GetAllowed() const;
		uintmax_t        GetDenied() const;


	private:
		Discord::Snowflake mID;
		int                mType;
		std::uintmax_t     mAllowed;
		std::uintmax_t     mDenied;
	};
}
