#pragma once


#include <nlohmann/json.hpp>
#include "Discord/Snowflake.hpp"
#include "Discord/Error.hpp"
#include "Discord/Entity/Guild.hpp"
#include "EventBase.hpp"
#include "Strawberry/Core/Util/Result.hpp"


namespace Strawberry::Discord::Event
{
	class GuildCreate
		: public EventBase
	{
	public:
		// Constructors
		static Core::Result<GuildCreate, Error> Parse(const nlohmann::json& json);


		[[nodiscard]] const Entity::Guild& GetGuild() const;


	private:
		GuildCreate() = default;


	private:
		Entity::Guild mGuild;
	};
}
