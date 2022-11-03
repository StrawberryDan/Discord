#pragma once



#include <nlohmann/json.hpp>
#include "Discord/Snowflake.hpp"
#include "Discord/Error.hpp"
#include "Discord/Entity/Guild.hpp"
#include "Base.hpp"
#include "Standard/Result.hpp"



namespace
{
	using Strawberry::Standard::Result;
}



namespace Strawberry::Discord::Event
{
	class GuildCreate
		: public Base
	{
	public:
		// Constructors
		static Result<GuildCreate, Error> Parse(const nlohmann::json& json);



		const Entity::Guild& GetGuild() const;



	private:
		GuildCreate() = default;



	private:
		Entity::Guild mGuild;
	};
}
