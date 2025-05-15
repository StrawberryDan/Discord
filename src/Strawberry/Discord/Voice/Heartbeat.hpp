#pragma once


#include <random>
#include <thread>


#include "Strawberry/Net/Websocket/Client.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Thread/RepeatingTask.hpp"


namespace Strawberry::Discord::Voice
{
	class Heartbeat
	{
	public:
		Heartbeat(Core::SharedMutex<Net::Websocket::WSSClient> wss, double interval);
		Heartbeat(const Heartbeat&) = delete;
		Heartbeat(Heartbeat&&) = delete;
		Heartbeat& operator=(const Heartbeat&) = delete;
		Heartbeat& operator=(Heartbeat&&) = delete;


		bool IsOk() const
		{
			return !mError.HasValue();
		}


		Net::Error GetError() const
		{
			return mError.Value();
		}

	private:
		void Tick();

		std::unique_ptr<std::random_device> mRandomDevice;
		const double mInterval;
		Core::SharedMutex<Net::Websocket::WSSClient> mWSS;
		Core::Optional<Core::RepeatingTask> mThread;
		Core::Optional<Net::Error> mError;


		std::chrono::steady_clock mClock;
		std::chrono::steady_clock::time_point mNextHeartbeatTime = mClock.now();
	};
} // namespace Strawberry::Discord::Voice
