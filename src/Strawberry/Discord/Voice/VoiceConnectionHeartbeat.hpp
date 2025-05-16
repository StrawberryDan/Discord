#pragma once


#include <random>
#include <thread>


#include "Strawberry/Net/Websocket/WebsocketClient.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Thread/RepeatingTask.hpp"


namespace Strawberry::Discord::Voice
{
	class VoiceConnectionHeartbeat
	{
	public:
		VoiceConnectionHeartbeat(Core::SharedMutex<Net::Websocket::WSSClient> wss, double interval);
		VoiceConnectionHeartbeat(const VoiceConnectionHeartbeat&) = delete;
		VoiceConnectionHeartbeat(VoiceConnectionHeartbeat&&) = delete;
		VoiceConnectionHeartbeat& operator=(const VoiceConnectionHeartbeat&) = delete;
		VoiceConnectionHeartbeat& operator=(VoiceConnectionHeartbeat&&) = delete;


		bool IsOk() const
		{
			return !mError.HasValue();
		}


		Net::Error GetError() const
		{
			return mError.Value();
		}

	private:
		void Tick();

		std::unique_ptr<std::random_device> mRandomDevice;
		const double mInterval;
		Core::SharedMutex<Net::Websocket::WSSClient> mWSS;
		Core::Optional<Core::RepeatingTask> mThread;
		Core::Optional<Net::Error> mError;


		std::chrono::steady_clock::time_point mNextHeartbeatTime = std::chrono::steady_clock::now();
	};
} // namespace Strawberry::Discord::Voice
