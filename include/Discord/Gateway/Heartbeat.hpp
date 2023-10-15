#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Core/Net/Websocket/Client.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Thread/RepeatingTask.hpp"
#include "Strawberry/Core/Timing/Clock.hpp"

namespace Strawberry::Discord::Gateway
{
	class Heartbeat
	{
	public:
		Heartbeat(Core::SharedMutex<Core::Net::Websocket::WSSClient> wss, double interval);

		void UpdateSequenceNumber(size_t value);

		[[nodiscard]] double GetInterval() const { return mInterval; }


	private:
		void Tick(uint32_t& count);


		const double                                       mInterval;
		Core::Clock                                        mClock;
		Core::SharedMutex<Core::Net::Websocket::WSSClient> mWSS;
		Core::Optional<Core::Mutex<size_t>>                mLastSequenceNumber;
		Core::Optional<Core::RepeatingTask>                mThread;
	};
} // namespace Strawberry::Discord::Gateway