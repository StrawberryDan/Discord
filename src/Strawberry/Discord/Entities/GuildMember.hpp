#pragma once
//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Discord/Error.hpp"
#include "Strawberry/Discord/Entities/User.hpp"
// Core
#include "Strawberry/Core/Types/Result.hpp"
// JSON
#include "nlohmann/json.hpp"

//======================================================================================================================
//		Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Discord::Entity
{
	class GuildMember
	{
	public:
		static Core::Result<GuildMember, Error> Parse(const nlohmann::json& json) noexcept;

	protected:
		GuildMember() = default;

	private:
		Core::Optional<User> mUser;
	};
}
