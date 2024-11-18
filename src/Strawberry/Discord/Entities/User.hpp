#pragma once
//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Discord/Snowflake.hpp"
#include "Strawberry/Discord/Error.hpp"
// Core
#include "Strawberry/Core/Types/Result.hpp"
// 3rd Party
#include "nlohmann/json.hpp"


//======================================================================================================================
//		Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Discord::Entity
{
	class User
	{
	public:
		static Core::Result<User, Error> Parse(const nlohmann::json& json);


		friend bool operator==(const User& a, const User& b) noexcept
		{
			return a.ID() == b.ID();
		}


		friend bool operator!=(const User& a, const User& b) noexcept
		{
			return !operator==(a, b);
		}


		Snowflake ID() const noexcept
		{
			return mID;
		}

	private:
		Snowflake mID;
	};
}
