#pragma once



#include <thread>



#include "Standard/Clock.hpp"
#include "Standard/Mutex.hpp"
#include "Standard/Net/Websocket/Client.hpp"



namespace Strawberry::Discord::Gateway
{
	class Heartbeat
	{
	public:
		Heartbeat(Standard::SharedMutex<Standard::Net::Websocket::WSSClient> wss, double interval);
		~Heartbeat();

		void UpdateSequenceNumber(size_t value);


		[[nodiscard]] double GetInterval() const { return mInterval; }



	private:
		void Run();

		std::thread                                                mThread;
		const double                                               mInterval;
		Standard::Clock                                            mClock;
		Standard::Mutex<bool>                                      mShouldStop;
		Standard::SharedMutex<Standard::Net::Websocket::WSSClient> mWSS;
		Standard::Option<Standard::Mutex<size_t>>                  mLastSequenceNumber;
	};
}