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
		std::string AsString() const;


	public:
		// Operators
		bool operator==(const Snowflake& rhs) const;
		bool operator!=(const Snowflake& rhs) const;
		uint64_t operator*() const;



	public:
		friend struct  std::hash<Snowflake>;



	private:
		// Members
		uint64_t mSnowflake;
	};
}



template <>
struct std::hash<Strawberry::Discord::Snowflake>
{
	std::size_t operator()(const Strawberry::Discord::Snowflake& v) const noexcept
	{
		return v.operator*();
	}
};