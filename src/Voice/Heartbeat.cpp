#include "Discord/Voice/Heartbeat.hpp"



#include <random>



namespace Strawberry::Discord::Voice
{
	Heartbeat::Heartbeat(Standard::SharedMutex<Standard::Net::Websocket::WSSClient> wss, double interval)
			: mWSS(wss)
			, mInterval(interval)
			, mShouldStop(false)
	{
		mThread = std::thread(&Heartbeat::Run, this);
	}



	Heartbeat::~Heartbeat()
	{
		(*mShouldStop.Lock()) = true;
		mThread.join();
	}



	void Heartbeat::Run()
	{
		std::random_device rd;
		std::uniform_real_distribution<double> jitterDist(0.0, 0.9 * mInterval);
		std::mt19937_64 rng(rd());
		double jitter = jitterDist(rng);

		mClock.Restart();
		while (mClock.Read() < jitter && !*mShouldStop.Lock())
		{
			std::this_thread::yield();
		}

		int count = 0;
		while (!*mShouldStop.Lock())
		{
			if (mClock.Read() > 0.9 * mInterval || count == 0)
			{
				nlohmann::json message;
				message["op"] = 3;
				uint64_t nonce = mRandomDevice();
				message["d"] = nonce;
				Standard::Net::Websocket::Message wssMessage(to_string(message));

				mWSS.Lock()->SendMessage(wssMessage);
				count += 1;
				mClock.Restart();
			}

			std::this_thread::yield();
		}
	}
}