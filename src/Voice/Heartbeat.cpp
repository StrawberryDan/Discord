#include "Discord/Voice/Heartbeat.hpp"



#include <random>



namespace Strawberry::Discord::Voice
{
	Heartbeat::Heartbeat(Core::SharedMutex<Core::Net::Websocket::WSSClient> wss, double interval)
			: mWSS(wss)
			, mInterval(interval)
			, mShouldStop(false)
			, mThread(std::make_unique<std::thread>(&Heartbeat::Run, this))
	{}



	Heartbeat::~Heartbeat()
	{
		(*mShouldStop.Lock()) = true;
		mThread->join();
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
				uint64_t nonce = rd();
				message["d"] = nonce;
				Core::Net::Websocket::Message wssMessage(to_string(message));

				auto sendResult = mWSS.Lock()->SendMessage(wssMessage);
				if (!sendResult && sendResult.Err() == Core::Net::Websocket::Error::Closed)
				{
					return;
				}
				else
				{
					sendResult.Unwrap();
				}

				count += 1;
				mClock.Restart();
			}

			std::this_thread::yield();
		}
	}
}