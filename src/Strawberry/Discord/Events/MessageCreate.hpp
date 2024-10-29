#pragma once
//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include <nlohmann/json.hpp>
#include "Strawberry/Core/Types/Result.hpp"
#include "Strawberry/Discord/Error.hpp"


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


		const std::string& Content() const noexcept
		{
			return mContents;
		}

	protected:
		MessageCreate() = default;

	private:
		std::string mContents;
	};
}