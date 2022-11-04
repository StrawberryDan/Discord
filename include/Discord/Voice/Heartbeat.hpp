#pragma once



#include <thread>
#include <random>



#include "Standard/Clock.hpp"
#include "Standard/Mutex.hpp"
#include "Standard/Net/Websocket/Client.hpp"



namespace Strawberry::Discord::Voice
{
	class Heartbeat
	{
	public:
		Heartbeat(Standard::SharedMutex<Standard::Net::Websocket::WSSClient> wss, double interval);
		~Heartbeat();



	private:
		void Run();

		std::thread                                                mThread;
		const double                                               mInterval;
		Standard::Clock                                            mClock;
		Standard::Mutex<bool>                                      mShouldStop;
		Standard::SharedMutex<Standard::Net::Websocket::WSSClient> mWSS;
		std::random_device                                         mRandomDevice;
	};
}