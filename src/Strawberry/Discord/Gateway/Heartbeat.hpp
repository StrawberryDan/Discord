#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Net/Websocket/Client.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Thread/RepeatingTask.hpp"

namespace Strawberry::Discord::Gateway
{
    class Heartbeat
    {
        public:
            Heartbeat(Core::SharedMutex<Net::Websocket::WSSClient> wss, double interval);
            Heartbeat(const Heartbeat& rhs)                = delete;
            Heartbeat& operator=(const Heartbeat& rhs)     = delete;
            Heartbeat(Heartbeat&& rhs) noexcept            = default;
            Heartbeat& operator=(Heartbeat&& rhs) noexcept = default;


            void UpdateSequenceNumber(size_t value);


            [[nodiscard]] double GetInterval() const
            {
                return mInterval;
            }


            bool       IsOk() const;
            Net::Error GetError() const;

        private:
            void Tick();


            const double                                 mInterval;
            Core::SharedMutex<Net::Websocket::WSSClient> mWSS;
            Core::Optional<Core::Mutex<size_t> >         mLastSequenceNumber;
            Core::Optional<Core::RepeatingTask>          mThread;
            Core::Optional<Net::Error>                   mError;


            std::chrono::steady_clock mClock;
            std::chrono::steady_clock::time_point mNextHeartbeatTime = mClock.now();
    };
} // namespace Strawberry::Discord::Gateway
