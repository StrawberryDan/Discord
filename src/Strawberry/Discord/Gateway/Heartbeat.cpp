#include "Strawberry/Discord/Gateway/Heartbeat.hpp"


#include <random>
#include <utility>

namespace Strawberry::Discord::Gateway
{
    Heartbeat::Heartbeat(Core::SharedMutex<Net::Websocket::WSSClient> wss, double interval)
        : mWSS(std::move(wss))
        , mInterval(interval)
    {
        auto startUp = [self = GetReflexivePointer()](Core::RepeatingTask* thread) mutable
        {
            std::random_device                     rd;
            std::uniform_real_distribution<double> jitterDist(0.0, 0.5);
            std::mt19937_64                        rng(rd());
            double                                 jitter = jitterDist(rng);

            self->mClock.Restart();
            while (self->mClock.Read() < jitter && thread->IsRunning())
            {
                std::this_thread::yield();
            }
        };

        mThread.Emplace(startUp,
                        [self = GetReflexivePointer(), count = uint32_t(0)]() mutable
                        {
                            self->Tick(count);
                        });
    }


    void Heartbeat::Tick(uint32_t& count)
    {
        if (mClock.Read() > 0.9 * mInterval || count == 0)
        {
            nlohmann::json message;
            message["op"] = 1;
            message["d"]  = mLastSequenceNumber ? nlohmann::json(*mLastSequenceNumber->Lock()) : nlohmann::json();
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
            mLastSequenceNumber = Core::Mutex(value);
        }
    }


    bool Heartbeat::IsOk() const
    {
        return !mError.HasValue();
    }


    Net::Error Heartbeat::GetError() const
    {
        return mError.Value();
    }
} // namespace Strawberry::Discord::Gateway
