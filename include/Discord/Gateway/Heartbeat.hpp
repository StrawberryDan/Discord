#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Net/Websocket/Client.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Thread/RepeatingTask.hpp"
#include "Strawberry/Core/Timing/Clock.hpp"

namespace Strawberry::Discord::Gateway
{
	class Heartbeat
	{
	public:
		Heartbeat(Core::SharedMutex<Net::Websocket::WSSClient> wss, double interval);
		Heartbeat(const Heartbeat& rhs) = delete;
		Heartbeat& operator=(const Heartbeat& rhs) = delete;
		Heartbeat(Heartbeat&& rhs) noexcept = delete;
		Heartbeat& operator=(Heartbeat&& rhs) noexcept = delete;


		void UpdateSequenceNumber(size_t value);


		[[nodiscard]] double GetInterval() const { return mInterval; }


	private:
		void Tick(uint32_t& count);


		const double                                       mInterval;
		Core::Clock                                        mClock;
		Core::SharedMutex<Net::Websocket::WSSClient>       mWSS;
		Core::Optional<Core::Mutex<size_t>>                mLastSequenceNumber;
		Core::Optional<Core::RepeatingTask>                mThread;
	};
} // namespace Strawberry::Discord::Gateway
