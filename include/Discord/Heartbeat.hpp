#pragma once



#include <thread>



#include "Standard/Clock.hpp"
#include "Standard/Mutex.hpp"
#include "Standard/Net/Websocket/Client.hpp"



namespace Strawberry::Discord
{
	using namespace Strawberry::Standard;
	using namespace Strawberry::Standard::Net;



	class Heartbeat
	{
	public:
		Heartbeat(SharedMutex<Websocket::WSSClient> wss, double interval);
		~Heartbeat();

		void UpdateSequenceNumber(size_t value);

	private:
		void Run();

		std::thread mThread;
		const double mInterval;
		Clock mClock;
		Mutex<bool> mShouldStop;
		SharedMutex<Websocket::WSSClient> mWSS;
		Option<Mutex<size_t>> mLastSequenceNumber;
	};
}