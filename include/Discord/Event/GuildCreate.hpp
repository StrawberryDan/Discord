#pragma once


#include "Discord/Entity/Guild.hpp"
#include "Discord/Error.hpp"
#include "Discord/Snowflake.hpp"
#include "EventBase.hpp"
#include "Strawberry/Core/Util/Result.hpp"
#include <nlohmann/json.hpp>


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
} // namespace Strawberry::Discord::Event
