#include "Discord/Snowflake.hpp"


#include <sstream>


namespace Strawberry::Discord
{
	Snowflake::Snowflake()
		: mSnowflake(0)
	{}


	Snowflake::Snowflake(uint64_t snowflake)
		: mSnowflake(snowflake)
	{}


	Snowflake::Snowflake(const std::string& snowflake)
		: mSnowflake(0)
	{
		std::stringstream ss(snowflake);
		ss >> mSnowflake;
	}


	std::string Snowflake::AsString() const
	{
		return std::to_string(mSnowflake);
	}


	bool Snowflake::operator==(const Snowflake& rhs) const
	{
		return mSnowflake == rhs.mSnowflake;
	}


	bool Snowflake::operator!=(const Snowflake& rhs) const
	{
		return mSnowflake == rhs.mSnowflake;
	}


	uint64_t Snowflake::operator*() const
	{
		return mSnowflake;
	}
}// namespace Strawberry::Discord