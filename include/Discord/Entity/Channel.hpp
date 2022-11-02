#pragma once



#include "nlohmann/json.hpp"
#include "Standard/Result.hpp"
#include "Discord/Error.hpp"
#include "Discord/Snowflake.hpp"



namespace Strawberry::Discord::Entity
{
	using namespace Strawberry::Standard;



	class Channel
	{
	public:
		enum class Type : uint8_t;



	public:
		static Result<Channel, Error> Parse(const nlohmann::json& json);



	public:



	private:
		Snowflake   mSnowflake;
		std::string mName;
		Type        mType;
		size_t      mPosition;
	};



	enum class Channel::Type : uint8_t
	{
		GUILD_TEXT          =  0,
		DM                  =  1,
		GUILD_VOICE         =  2,
		GROUP_DM            =  3,
		GUILD_CATEGORY      =  4,
		GUILD_ANNOUNCEMENT  =  5,
		ANNOUNCEMENT_THREAD = 10,
		PUBLIC_THREAD       = 11,
		PRIVATE_THREAD      = 12,
		GUILD_STAGE_VOICE   = 13,
		GUILD_DIRECTORY     = 14,
		GUILD_FORUM         = 15,
	};
}