#pragma once



#include <thread>



#include "Core/Clock.hpp"
#include "Core/Mutex.hpp"
#include "Core/Net/Websocket/Client.hpp"



namespace Strawberry::Discord::Gateway
{
	class Heartbeat
	{
	public:
		Heartbeat(Core::SharedMutex<Core::Net::Websocket::WSSClient> wss, double interval);
		~Heartbeat();

		void UpdateSequenceNumber(size_t value);


		[[nodiscard]] double GetInterval() const { return mInterval; }



	private:
		void Run();

		std::thread                                                mThread;
		const double                                               mInterval;
		Core::Clock                                            mClock;
		Core::Mutex<bool>                                      mShouldStop;
		Core::SharedMutex<Core::Net::Websocket::WSSClient> mWSS;
		Core::Option<Core::Mutex<size_t>>                  mLastSequenceNumber;
	};
}