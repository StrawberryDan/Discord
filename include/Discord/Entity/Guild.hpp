#pragma once



#include "Channel.hpp"
#include "Strawberry/Core/Result.hpp"
#include "Discord/Error.hpp"
#include "Discord/Snowflake.hpp"
#include "nlohmann/json.hpp"

#include <string>
#include <unordered_set>



namespace Strawberry::Discord::Entity
{
	class Guild
	{
	public:
		static Core::Result<Guild, Error>		Parse(const nlohmann::json& json);



	public:
		inline const Snowflake&					GetId() const { return mGuildId; }
		inline const std::string&				GetName() const { return mGuildName; }




	private:
		Snowflake								mGuildId;
		std::string								mGuildName;
	};
}
