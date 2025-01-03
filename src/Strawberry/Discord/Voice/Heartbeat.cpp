#include "Strawberry/Discord/Voice/Heartbeat.hpp"


#include <random>
#include <utility>


namespace Strawberry::Discord::Voice
{
    Heartbeat::Heartbeat(Core::SharedMutex<Net::Websocket::WSSClient> wss, double interval)
        : mWSS(std::move(wss))
        , mInterval(interval)
        , mRandomDevice(std::make_unique<std::random_device>())
    {
        auto startUp = [this](Core::RepeatingTask* thread) mutable
        {
            std::uniform_real_distribution<double> jitterDist(0.0, 0.9 * mInterval);
            std::mt19937_64                        rng((*mRandomDevice)());
            double                                 jitter = jitterDist(rng);

            mClock.Restart();
            while (mClock.Read() < jitter && thread->IsRunning())
            {
                std::this_thread::yield();
            }
        };

        mThread.Emplace(startUp,
                        [this, count = uint32_t(0)]() mutable
                        {
                            Tick(count);
                        });
    }


    void Heartbeat::Tick(uint32_t& count)
    {
        if (mClock.Read() > 0.9 * mInterval || count == 0)
        {
            nlohmann::json message;
            message["op"]  = 3;
            uint64_t nonce = (*mRandomDevice)();
            message["d"]   = nonce;
            Net::Websocket::Message wssMessage(to_string(message));

            auto sendResult = mWSS.Lock()->SendMessage(wssMessage);
            if (!sendResult && sendResult.Err() == Net::Error::ConnectionReset)
            {
                return;
            }
            else if (!sendResult)
            {
                mError = sendResult.Err();
                return;
            }

            count += 1;
            mClock.Restart();
        }

        std::this_thread::yield();
    }
} // namespace Strawberry::Discord::Voice
