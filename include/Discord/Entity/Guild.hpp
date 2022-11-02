#pragma once



#include "Discord/Snowflake.hpp"
#include "Channel.hpp"
#include "Discord/Error.hpp"
#include "nlohmann/json.hpp"
#include "Standard/Result.hpp"
#include <string>



namespace Strawberry::Discord::Entity
{
	class Guild
	{
	public:
		static Result<Guild, Error> Parse(const nlohmann::json& json);



	public:
		const Snowflake& Id() const { return mGuildId; }
		const std::string& Name() const { return mGuildName; }



	private:
		Snowflake            mGuildId;
		std::string          mGuildName;
		std::vector<Channel> mChannels;
	};
}