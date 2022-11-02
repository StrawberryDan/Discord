#pragma once



#include <cstdint>
#include <string>



namespace Strawberry::Discord
{
	class Snowflake
	{
	public:
		// Constructors
		Snowflake();
		explicit Snowflake(uint64_t snowflake);
		explicit Snowflake(const std::string& snowflake);

		// Methods



	public:
		// Operators
		bool operator==(const Snowflake& rhs) const;
		bool operator!=(const Snowflake& rhs) const;
		uint64_t operator*() const;



	private:
		// Members
		uint64_t mSnowflake;
	};
}