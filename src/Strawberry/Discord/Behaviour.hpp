#pragma once


#include "Strawberry/Discord/Events/GuildCreate.hpp"
#include "Strawberry/Discord/Events/Ready.hpp"


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
} // namespace Strawberry::Discord
