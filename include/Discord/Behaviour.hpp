#pragma once


#include "Discord/Event/GuildCreate.hpp"
#include "Discord/Event/Ready.hpp"


namespace Strawberry::Discord
{
	class Behaviour
	{
	public:
		virtual ~Behaviour() = default;


	public:
		virtual void OnReady(const Event::Ready& event) {}


		virtual void OnGuildCreate(const Event::GuildCreate& event) {}
	};
}
