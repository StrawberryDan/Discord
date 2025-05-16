#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Discord
#include "Strawberry/Discord/Intent.hpp"
#include "GatewayConnectionHeartbeat.hpp"
// Strawberry Core
#include "Strawberry/Net/Endpoint.hpp"
#include "Strawberry/Net/Websocket/Client.hpp"
#include "Strawberry/Core/Types/Variant.hpp"
// Standard Library
#include <queue>

namespace Strawberry::Discord::Gateway
{
    class Gateway
    {
        public:
            using ReceiveResult = Core::Result<Net::Websocket::Message, Net::Error>;
            using SendResult    = Core::Result<void, Net::Error>;

        public:
            static Core::Optional<Gateway> Connect(const Net::Endpoint& endpoint, const std::string& token, Intent intents);

            Gateway(Gateway&&) = default;

            bool IsOk() const;

            void Resume();

            /**
             * @brief Receives a message from the gateway.
             * @param checkBuffer Whether the queue of buffered messages should be checked.
             * @return A message which is either received from the gateway or is form the internal buffer queue.
             */
            ReceiveResult Receive(bool checkBuffer = true);

            /**
             * @brief Sends a websocket message to the gateway.
             * @param msg The message to send/
             * @return Either the number of bytes transmitted, or an error if one occurred.
             */
            SendResult Send(const Net::Websocket::Message& msg);


            /// Returns the number of buffered messages in the internal queue.
            size_t BufferedMessageCount();
            /// Stores a message in the internal buffer so that it can be received again by another source.
            void BufferMessage(Net::Websocket::Message message);


            [[nodiscard]] double GetHeartbeatInterval() const
            {
                return mHeartbeat->GetInterval();
            }


            void SetResumeGatewayURL(const std::string& url);

        private:
            /// Private Constructor
            Gateway(const Net::Endpoint& endpoint, const std::string& token, Intent intents);


            Core::SharedMutex<Net::Websocket::WSSClient> mWSS;
            std::unique_ptr<GatewayConnectionHeartbeat>                   mHeartbeat;
            std::queue<Net::Websocket::Message>          mMessageBuffer;
            std::string                                  mResumeEndpoint;


            std::string mToken;
            Intent      mIntent;
    };
} // namespace Strawberry::Discord::Gateway
