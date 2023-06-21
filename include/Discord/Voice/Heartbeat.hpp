#pragma once



#include <thread>
#include <random>



#include "Strawberry/Core/Clock.hpp"
#include "Strawberry/Core/Mutex.hpp"
#include "Strawberry/Core/Net/Websocket/Client.hpp"



namespace Strawberry::Discord::Voice
{
	class Heartbeat
	{
	public:
		Heartbeat(Core::SharedMutex<Core::Net::Websocket::WSSClient> wss, double interval);
		Heartbeat(const Heartbeat&)				= delete;
		Heartbeat(Heartbeat&&)					= default;
		Heartbeat& operator=(const Heartbeat&)	= delete;
		Heartbeat& operator=(Heartbeat&&)		= delete;
		~Heartbeat();



	private:
		void Run();

		const double										mInterval;
		Core::Clock											mClock;
		Core::Mutex<bool>									mShouldStop;
		Core::SharedMutex<Core::Net::Websocket::WSSClient>	mWSS;
		std::unique_ptr<std::thread>						mThread;
	};
}