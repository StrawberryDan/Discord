#include "Discord/Gateway/Heartbeat.hpp"



#include <random>
#include <utility>



namespace Strawberry::Discord::Gateway
{
	Heartbeat::Heartbeat(Core::SharedMutex<Core::Net::Websocket::WSSClient> wss, double interval)
			: mWSS(std::move(wss))
			, mInterval(interval)
	{
		mStartUp = std::async( std::launch::async, [this]()
		{
			std::random_device rd;
			std::uniform_real_distribution<double> jitterDist(0.0, 0.9 * mInterval);
			std::mt19937_64 rng(rd());
			double jitter = jitterDist(rng);

			mClock.Restart();
			while (mClock.Read() < jitter)
			{
				std::this_thread::yield();
			}

			mThread.Emplace([this, count = uint32_t(0)] () mutable { Run(count); });
		});
	}


	Heartbeat::~Heartbeat()
	{
		mStartUp.wait();
	}


	void Heartbeat::Run(uint32_t& count)
	{
		if (mClock.Read() > 0.9 * mInterval || count == 0)
		{
			nlohmann::json message;
			message["op"] = 1;
			message["d"] = mLastSequenceNumber ? nlohmann::json(*mLastSequenceNumber->Lock()) : nlohmann::json();
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