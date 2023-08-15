#include "Discord/EventListener.hpp"


namespace Strawberry::Discord
{
	EventListener::EventListener()
		: mRegistry(nullptr)
	{}


	EventListener::EventListener(EventListener&& other) noexcept
		: mRegistry(std::move(other.mRegistry))
	{
		auto registry = mRegistry.Lock();
		registry->erase(&other);
		registry->insert(this);
	}


	EventListener& EventListener::operator=(EventListener&& other) noexcept
	{
		if (this != &other)
		{
			std::destroy_at(this);
			std::construct_at(&mRegistry, std::move(other.mRegistry));

			auto registry = mRegistry.Lock();
			registry->erase(&other);
			registry->insert(this);
		}

		return *this;
	}


	EventListener::~EventListener()
	{
		if (mRegistry)
		{
			auto registry = mRegistry.Lock();
			registry->erase(this);
		}
	}
}// namespace Strawberry::Discord