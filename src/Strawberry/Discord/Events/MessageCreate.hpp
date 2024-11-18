#pragma once
//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include <nlohmann/json.hpp>
#include "Strawberry/Core/Types/Result.hpp"
#include "Strawberry/Discord/Error.hpp"
#include "Strawberry/Discord/Entities/Channel.hpp"
#include "Strawberry/Discord/Entities/User.hpp"


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


		const Core::Optional<Snowflake>& GetGuildID() const noexcept
		{
			return mGuildID;
		}


		const Entity::User& GetAuthor() const noexcept
		{
			return mAuthor;
		}


		const Snowflake& GetChannelID() const noexcept
		{
			return mChannelID;
		}


		const std::string& GetContent() const noexcept
		{
			return mContents;
		}

	protected:
		MessageCreate() = default;

	private:
		Core::Optional<Snowflake> mGuildID;
		Entity::User              mAuthor;
		Snowflake                 mChannelID;
		std::string               mContents;
	};
}
