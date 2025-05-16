#include "Strawberry/Discord/Gateway/GatewayConnectionHeartbeat.hpp"


#include <random>
#include <utility>

namespace Strawberry::Discord::Gateway
{
	GatewayConnectionHeartbeat::GatewayConnectionHeartbeat(Core::SharedMutex<Net::Websocket::WSSClient> wss, double interval)
		: mWSS(std::move(wss))
		  , mInterval(interval)
	{
		auto startUp = [this]() mutable
		{
			std::random_device rd;
			std::uniform_real_distribution jitterDist(0.0, 0.5);
			std::mt19937_64 rng(rd());
			std::chrono::duration<double> jitter(jitterDist(rng));


			mNextHeartbeatTime = std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::steady_clock::duration>(jitter);
		};

		mThread.Emplace(
			[this]() mutable
			{
				Tick();
			},
			std::move(startUp));
	}


	void GatewayConnectionHeartbeat::Tick()
	{
		if (std::chrono::steady_clock::now() > mNextHeartbeatTime)
		{
			nlohmann::json message;
			message["op"] = 1;
			message["d"] = mLastSequenceNumber ? nlohmann::json(*mLastSequenceNumber->Lock()) : nlohmann::json();
			Net::Websocket::Message wssMessage(message.dump(1, '\t'));

			auto sendResult = mWSS.Lock()->SendMessage(wssMessage);
			if (!sendResult && sendResult.Err() == Net::Error::ConnectionReset)
			{
				Core::Logging::Warning("WSS Connection reset in gateway when sending heartbeat!");
				return;
			}
			else if (!sendResult)
			{
				mError = sendResult.Err();
				Core::Logging::Warning("WSS send error in gateway when sending heartbeat!");
				return;
			}

			mNextHeartbeatTime = std::chrono::steady_clock::now() + duration_cast<std::chrono::steady_clock::duration>(
				std::chrono::duration<double>(mInterval));
		}
		else
		{
			std::this_thread::sleep_until(mNextHeartbeatTime);
		}
	}


	void GatewayConnectionHeartbeat::UpdateSequenceNumber(size_t value)
	{
		if (mLastSequenceNumber)
		{
			(*mLastSequenceNumber->Lock()) = value;
		}
		else
		{
			mLastSequenceNumber = Core::Mutex(value);
		}
	}


	bool GatewayConnectionHeartbeat::IsOk() const
	{
		return !mError.HasValue();
	}


	Net::Error GatewayConnectionHeartbeat::GetError() const
	{
		return mError.Value();
	}
} // namespace Strawberry::Discord::Gateway
