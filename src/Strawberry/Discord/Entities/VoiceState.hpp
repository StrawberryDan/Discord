#pragma once
//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Discord/Entities/GuildMember.hpp"
#include "Strawberry/Discord/Snowflake.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
#include "Strawberry/Core/Types/Result.hpp"
#include "nlohmann/json.hpp"

//======================================================================================================================
//		Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Discord::Entity
{
	class VoiceState
	{
	public:
		static Core::Result<VoiceState, Error> Parse(const nlohmann::json& json) noexcept;

		auto&& GetGuild(this auto&& self)
		{
			return self.mGuildID;
		}

		auto&& GetChannel(this auto&& self)
		{
			return self.mChannelID;
		}

	protected:
		VoiceState() = default;

	private:
		Core::Optional<Snowflake>   mGuildID;
		Core::Optional<Snowflake>   mChannelID;
		Core::Optional<Snowflake>   mUserID;
		Core::Optional<GuildMember> mGuildMember;
	};
}
