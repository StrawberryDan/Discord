#pragma once



#include "Discord/Snowflake.hpp"
#include "Channel.hpp"
#include "Discord/Error.hpp"
#include "nlohmann/json.hpp"
#include "Core/Result.hpp"
#include <string>



namespace Strawberry::Discord::Entity
{
	class Guild
	{
	public:
		using ChannelList = std::vector<Channel>;



	public:
		static Core::Result<Guild, Error> Parse(const nlohmann::json& json);



	public:
		const Snowflake&   GetId() const { return mGuildId; }
		const std::string& GetName() const { return mGuildName; }
		const ChannelList& GetChannels() const { return mChannels; }




	private:
		Snowflake            mGuildId;
		std::string          mGuildName;
		ChannelList          mChannels;
	};
}