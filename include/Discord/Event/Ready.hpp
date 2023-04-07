#pragma once



#include "Base.hpp"
#include "Discord/Snowflake.hpp"
#include "Core/Result.hpp"
#include "nlohmann/json.hpp"
#include "Discord/Error.hpp"



namespace Strawberry::Discord::Event
{
	class Ready
		: public Base
	{
	public:
		static Core::Result<Ready, Error> Parse(const nlohmann::json& json);



	public:
		Snowflake GetUserId() const;



	private:
		Snowflake mUserId;
	};
}
