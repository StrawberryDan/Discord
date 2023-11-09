#pragma once


#include <random>
#include <thread>


#include "Strawberry/Net/Websocket/Client.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Thread/RepeatingTask.hpp"
#include "Strawberry/Core/Timing/Clock.hpp"


namespace Strawberry::Discord::Voice
{
	class Heartbeat
	{
	public:
		Heartbeat(Core::SharedMutex<Net::Websocket::WSSClient> wss, double interval);
		Heartbeat(const Heartbeat&)            = delete;
		Heartbeat(Heartbeat&&)                 = delete;
		Heartbeat& operator=(const Heartbeat&) = delete;
		Heartbeat& operator=(Heartbeat&&)      = delete;


	private:
		void Tick(uint32_t& count);

		std::unique_ptr<std::random_device>                mRandomDevice;
		const double                                       mInterval;
		Core::Clock                                        mClock;
		Core::SharedMutex<Net::Websocket::WSSClient> mWSS;
		Core::Optional<Core::RepeatingTask>                mThread;
	};
} // namespace Strawberry::Discord::Voice
