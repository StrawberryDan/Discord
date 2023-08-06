#pragma once



#include <concepts>



#include "Strawberry/Core/Util/Option.hpp"



namespace Strawberry::Discord::Event
{
	class EventBase
	{
	public:
		virtual ~EventBase() = default;

		template<typename T>
		bool IsType() const requires(std::derived_from<T, EventBase>);

		template<typename T>
		Core::Option<T*> Cast() requires(std::derived_from<T, EventBase>);

		template<typename T>
		Core::Option<const T*> Cast() const requires(std::derived_from<T, EventBase>);
	};
}



namespace Strawberry::Discord::Event
{
	template <typename T>
	bool EventBase::IsType() const requires(std::derived_from<T, EventBase>)
	{
		return dynamic_cast<const T*>(this) != nullptr;
	}



	template <typename T>
	Core::Option<T*> EventBase::Cast() requires (std::derived_from<T, EventBase>)
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
	Core::Option<const T*> EventBase::Cast() const requires (std::derived_from<T, EventBase>)
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