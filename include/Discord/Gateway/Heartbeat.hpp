#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Core/Clock.hpp"
#include "Strawberry/Core/LoopingThread.hpp"
#include "Strawberry/Core/Mutex.hpp"
#include "Strawberry/Core/Net/Websocket/Client.hpp"



namespace Strawberry::Discord::Gateway
{
	class Heartbeat
	{
	public:
		Heartbeat(Core::SharedMutex<Core::Net::Websocket::WSSClient> wss, double interval);
		~Heartbeat();


		void UpdateSequenceNumber(size_t value);


		[[nodiscard]] double GetInterval() const { return mInterval; }



	private:
		void Run(uint32_t& count);

		const double                                       mInterval;
		Core::Clock                                        mClock;
		Core::SharedMutex<Core::Net::Websocket::WSSClient> mWSS;
		Core::Option<Core::Mutex<size_t>>                  mLastSequenceNumber;
		std::future<void>                                  mStartUp;
		Core::Option<Core::LoopingThread>                  mThread;
	};
}