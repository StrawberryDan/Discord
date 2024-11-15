#pragma once
//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Discord/Snowflake.hpp"
// Core
#include "Strawberry/Core/Types/Optional.hpp"
// 3rd Party
#include "nlohmann/json.hpp"


//======================================================================================================================
//		Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Discord
{
	class User
	{
	public:
		static Core::Optional<User> Parse(const nlohmann::json& json);


		Snowflake ID() const noexcept
		{
			return mID;
		}

	private:
		Snowflake mID;
	};
}
