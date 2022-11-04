#pragma once



#include <concepts>



#include "Standard/Option.hpp"



namespace Strawberry::Discord::Event
{
	class Base
	{
	public:
		virtual ~Base() = default;

		template<typename T>
		bool IsType() const requires(std::derived_from<T, Base>);

		template<typename T>
		Standard::Option<T*> Cast() requires(std::derived_from<T, Base>);

		template<typename T>
		Standard::Option<const T*> Cast() const requires(std::derived_from<T, Base>);
	};
}



namespace Strawberry::Discord::Event
{
	template <typename T>
	bool Base::IsType() const requires(std::derived_from<T, Base>)
	{
		return dynamic_cast<const T*>(this) != nullptr;
	}



	template <typename T>
	Standard::Option<T*> Base::Cast() requires (std::derived_from<T, Base>)
	{
		auto ptr = dynamic_cast<T*>(this);
		if (ptr)
		{
			return ptr;
		}
		else
		{
			return {};
		}
	}



	template <typename T>
	Standard::Option<const T*> Base::Cast() const requires (std::derived_from<T, Base>)
	{
		auto ptr = dynamic_cast<const T*>(this);
		if (ptr)
		{
			return ptr;
		}
		else
		{
			return {};
		}
	}
}