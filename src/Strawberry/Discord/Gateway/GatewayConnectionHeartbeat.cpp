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
		const auto NOW = std::chrono::steady_clock::now();

		if (NOW > mNextHeartbeatTime)
		{
			Net::Websocket::Message heartBeatMessage = CreateHeartbeatMessage();
			auto sendResult = mWSS.Lock()->SendMessage(heartBeatMessage);

			if (!sendResult && sendResult.Err().IsType<Net::ErrorConnectionReset>())
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

			mNextHeartbeatTime = NOW + duration_cast<std::chrono::steady_clock::duration>(
				std::chrono::duration<double>(mInterval));
		}
		else
		{
			std::this_thread::sleep_until(mNextHeartbeatTime);
		}
	}


	Net::Websocket::Message GatewayConnectionHeartbeat::CreateHeartbeatMessage()
	{
		auto lastSequenceNumber = mLastSequenceNumber.Lock();

		nlohmann::json json;
		json["op"] = 1;
		json["d"] = nlohmann::json(nullptr);
		if (lastSequenceNumber->HasValue()) json["d"] = lastSequenceNumber->Value();
		Net::Websocket::Message message(json.dump());
		return message;
	}


	void GatewayConnectionHeartbeat::UpdateSequenceNumber(size_t value)
	{
		auto lastSequenceNumber = mLastSequenceNumber.Lock();
		*lastSequenceNumber = value;
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
