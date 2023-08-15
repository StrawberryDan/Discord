#pragma once


#include "Event/EventBase.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
#include <set>


namespace Strawberry::Discord
{
	class EventListener
	{
		friend class Bot;


	public:
		EventListener();
		EventListener(EventListener&& other) noexcept;
		EventListener& operator=(EventListener&& other) noexcept;
		virtual ~EventListener();

		virtual void ProcessEvent(const Event::EventBase& event) = 0;

	private:
		Core::SharedMutex<std::set<EventListener*>> mRegistry;
	};
}// namespace Strawberry::Discord
