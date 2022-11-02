#pragma once



#include <thread>



#include "Standard/Clock.hpp"
#include "Standard/Mutex.hpp"
#include "Standard/Net/Websocket/Client.hpp"



namespace
{
	using Strawberry::Standard::Clock;
	using Strawberry::Standard::Mutex;
	using Strawberry::Standard::Option;
	using Strawberry::Standard::SharedMutex;
	using Strawberry::Standard::Net::Websocket::WSSClient;
}



namespace Strawberry::Discord
{
	class Heartbeat
	{
	public:
		Heartbeat(SharedMutex<WSSClient> wss, double interval);
		~Heartbeat();

		void UpdateSequenceNumber(size_t value);

	private:
		void Run();

		std::thread mThread;
		const double mInterval;
		Clock mClock;
		Mutex<bool> mShouldStop;
		SharedMutex<WSSClient> mWSS;
		Option<Mutex<size_t>> mLastSequenceNumber;
	};
}