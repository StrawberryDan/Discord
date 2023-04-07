#include "Discord/Gateway/Heartbeat.hpp"



#include <random>
#include <utility>



namespace Strawberry::Discord::Gateway
{
	Heartbeat::Heartbeat(Core::SharedMutex<Core::Net::Websocket::WSSClient> wss, double interval)
			: mWSS(std::move(wss))
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
				message["op"] = 1;
				message["d"] = mLastSequenceNumber ? nlohmann::json(*mLastSequenceNumber->Lock()) : nlohmann::json();
				Core::Net::Websocket::Message wssMessage(to_string(message));

				mWSS.Lock()->SendMessage(wssMessage);
				count += 1;
				mClock.Restart();
			}

			std::this_thread::yield();
		}
	}



	void Heartbeat::UpdateSequenceNumber(size_t value)
	{
		if (mLastSequenceNumber)
		{
			(*mLastSequenceNumber->Lock()) = value;
		}
		else
		{
			mLastSequenceNumber = value;
		}
	}
}