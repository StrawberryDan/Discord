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
    class GatewayConnectionHeartbeat
    {
        public:
            GatewayConnectionHeartbeat(Core::SharedMutex<Net::Websocket::WSSClient> wss, double interval);
            GatewayConnectionHeartbeat(const GatewayConnectionHeartbeat& rhs)                = delete;
            GatewayConnectionHeartbeat& operator=(const GatewayConnectionHeartbeat& rhs)     = delete;
            GatewayConnectionHeartbeat(GatewayConnectionHeartbeat&& rhs) noexcept            = default;
            GatewayConnectionHeartbeat& operator=(GatewayConnectionHeartbeat&& rhs) noexcept = default;


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


            std::chrono::steady_clock::time_point mNextHeartbeatTime = std::chrono::steady_clock::now();
    };
} // namespace Strawberry::Discord::Gateway
