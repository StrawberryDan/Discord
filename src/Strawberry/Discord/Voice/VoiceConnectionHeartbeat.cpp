#include "Strawberry/Discord/Voice/VoiceConnectionHeartbeat.hpp"


#include <random>
#include <utility>


namespace Strawberry::Discord::Voice
{
	VoiceConnectionHeartbeat::VoiceConnectionHeartbeat(Core::SharedMutex<Net::Websocket::WSSClient> wss, double interval)
		: mWSS(std::move(wss))
		  , mInterval(interval)
		  , mRandomDevice(std::make_unique<std::random_device>())
	{
		auto startUp = [this]() mutable
		{
			std::uniform_real_distribution<double> jitterDist(0.0, 0.9 * mInterval);
			std::mt19937_64 rng((*mRandomDevice)());
			double jitter = jitterDist(rng);

			mNextHeartbeatTime = std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
				std::chrono::duration<double>(jitter));
		};

		mThread.Emplace(
			[this]() mutable
			{
				Tick();
			},
			std::move(startUp));
	}


	void VoiceConnectionHeartbeat::Tick()
	{
		if (std::chrono::steady_clock::now() > mNextHeartbeatTime)
		{
			nlohmann::json message;
			message["op"] = 3;
			uint64_t nonce = (*mRandomDevice)();
			message["d"] = nonce;
			Net::Websocket::Message wssMessage(to_string(message));

			auto sendResult = mWSS.Lock()->SendMessage(wssMessage);
			if (!sendResult && sendResult.Err().template IsType<Net::ErrorConnectionReset>())
			{
				return;
			}
			else if (!sendResult)
			{
				mError = sendResult.Err();
				return;
			}

			mNextHeartbeatTime = std::chrono::steady_clock::now() + duration_cast<std::chrono::steady_clock::duration>(
				std::chrono::duration<double>(0.9 * mInterval));
		}
		else
		{
			std::this_thread::sleep_until(mNextHeartbeatTime);
		}
	}
} // namespace Strawberry::Discord::Voice
