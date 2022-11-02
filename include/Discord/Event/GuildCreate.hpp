#pragma once



#include <nlohmann/json.hpp>
#include "Discord/Snowflake.hpp"
#include "Discord/Error.hpp"
#include "Standard/Result.hpp"
#include "Discord/Entity/Guild.hpp"



namespace Strawberry::Discord::Event
{
	using namespace Strawberry::Standard;



	class GuildCreate
	{
	public:
		// Constructors
		static Result<GuildCreate, Error> Parse(const nlohmann::json& json);



	private:
		GuildCreate() = default;



	private:
		Entity::Guild mGuild;
	};
}
