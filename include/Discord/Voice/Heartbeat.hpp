#pragma once


#include <random>
#include <thread>


#include "Strawberry/Core/Net/Websocket/Client.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Thread/RepeatingTask.hpp"
#include "Strawberry/Core/Util/Clock.hpp"


namespace Strawberry::Discord::Voice
{
	class Heartbeat
	{
	public:
		Heartbeat(Core::SharedMutex<Core::Net::Websocket::WSSClient> wss, double interval);
		Heartbeat(const Heartbeat&)            = delete;
		Heartbeat(Heartbeat&&)                 = delete;
		Heartbeat& operator=(const Heartbeat&) = delete;
		Heartbeat& operator=(Heartbeat&&)      = delete;


	private:
		void Tick(uint32_t& count);

		std::unique_ptr<std::random_device>                mRandomDevice;
		const double                                       mInterval;
		Core::Clock                                        mClock;
		Core::SharedMutex<Core::Net::Websocket::WSSClient> mWSS;
		Core::Option<Core::RepeatingTask>                  mThread;
	};
}// namespace Strawberry::Discord::Voice