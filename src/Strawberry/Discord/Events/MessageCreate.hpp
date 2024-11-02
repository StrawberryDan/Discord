#pragma once
//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include <nlohmann/json.hpp>
#include "Strawberry/Core/Types/Result.hpp"
#include "Strawberry/Discord/Error.hpp"
#include "Strawberry/Discord/Snowflake.hpp"


//======================================================================================================================
//		Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Discord::Event
{
	class MessageCreate
	{
	public:
		// Constructors
		static Core::Result<MessageCreate, Error> Parse(const nlohmann::json& json);


		const Snowflake& Channel() const noexcept
		{
			return mChannel;
		}


		const std::string& Content() const noexcept
		{
			return mContents;
		}

	protected:
		MessageCreate() = default;

	private:
		Snowflake mAuthor;
		Snowflake mChannel;
		std::string mContents;
	};
}