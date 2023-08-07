#pragma once



#include "nlohmann/json.hpp"
#include "Strawberry/Core/Util/Result.hpp"
#include "Strawberry/Core/Util/Option.hpp"
#include "Discord/Error.hpp"
#include "PermissionOverwrite.hpp"
#include "Discord/Snowflake.hpp"



namespace Strawberry::Discord::Entity
{
	class Channel
	{
		friend class Guild;

	public:
		enum class Type : uint8_t;



	public:
		static Core::Result<Channel, Error> Parse(const nlohmann::json& json);



	public:
		const Snowflake& 	GetId() const		{ return mId; }
		const std::string&	GetName() const		{ return mName; }
		const Type&			GetType() const		{ return mType; }
		const size_t		GetPosition() const	{ return mPosition; }
		const auto&			GetGuild() const	{ return mGuildId; }


	private:
		Snowflake                        mId;
		std::string                      mName;
		Type                             mType;
		size_t                           mPosition;
		Core::Option<Snowflake>          mGuildId;
		std::vector<PermissionOverwrite> mPermissionOverwrites;
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