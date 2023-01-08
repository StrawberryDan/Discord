#pragma once



#include <variant>



namespace Strawberry::Discord
{
	class Error
	{
	public:
		enum class Type;
		using Payload = std::variant
		<
			std::monostate
		>;



	public:
		Error(Type type)
			: mType(type)
			, mPayload()
		{}

		template<typename T>
		Error(Type type, T data)
			: mType(type)
			, mPayload(data)
		{}



		Type GetType() const
		{
			return mType;
		}



		template<typename T>
		T GetData()
		{
			return std::get<T>(mPayload);
		}



	private:
		Type mType;
		Payload mPayload;
	};



	enum class Error::Type
	{
		InvalidJSON,
	};
}