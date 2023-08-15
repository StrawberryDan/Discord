#pragma once


#include "Channel.hpp"
#include "Strawberry/Core/Util/Result.hpp"
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
		static Core::Result<Guild, Error> Parse(const nlohmann::json& json);


	public:
		[[nodiscard]] inline const Snowflake& GetId() const { return mID; }


		[[nodiscard]] inline const std::string& GetName() const { return mName; }


		[[nodiscard]] inline Core::Option<std::uintmax_t> GetPermissions() const { return mPermissions; }


	private:
		Snowflake mID;
		std::string mName;
		Core::Option<std::uintmax_t> mPermissions;
	};
}
