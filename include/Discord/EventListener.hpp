#pragma once



#include "Event/EventBase.hpp"
#include "Strawberry/Core/Mutex.hpp"
#include <set>



namespace Strawberry::Discord
{
	class EventListener
	{
		friend class Bot;

	public:
		EventListener();
		EventListener(EventListener&& other);
		EventListener& operator=(EventListener&& other);
		virtual ~EventListener();

		virtual void ProcessEvent(const Event::EventBase& event) = 0;

	private:
		Core::SharedMutex<std::set<EventListener*>> mRegistry;
	};
}
