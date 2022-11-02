#pragma once



#include "Event/Base.hpp"



namespace Strawberry::Discord
{
	class EventListener
	{
	public:
		virtual void ProcessEvent(const Event::Base& event) = 0;
	};
}
