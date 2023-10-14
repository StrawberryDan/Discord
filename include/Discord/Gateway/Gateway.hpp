#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Discord
#include "Discord/Intent.hpp"
#include "Heartbeat.hpp"
// Strawberry Core
#include "Strawberry/Core/Net/Endpoint.hpp"
#include "Strawberry/Core/Net/Websocket/Client.hpp"
#include "Strawberry/Core/Util/Result.hpp"
// Standard Library
#include <queue>

namespace Strawberry::Discord::Gateway
{
	class Gateway
	{
	public:
		using ReceiveResult = Core::Result<Core::Net::Websocket::Message, Core::Net::Websocket::Error>;
		using SendResult = Core::Result<Core::NullType, Core::Net::Websocket::Error>;


	public:
		static Core::Optional<Gateway>
		Connect(const Core::Net::Endpoint& endpoint, const std::string& token, Intent intents);

		Gateway(Gateway&&) = default;

		bool IsOk() const;


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
		SendResult Send(const Core::Net::Websocket::Message& msg);


		/// Returns the number of buffered messages in the internal queue.
		size_t BufferedMessageCount();
		/// Stores a message in the internal buffer so that it can be received again by another source.
		void   BufferMessage(Core::Net::Websocket::Message message);

		[[nodiscard]] double GetHeartbeatInterval() const { return mHeartbeat->GetInterval(); }


	private:
		/// Private Constructor
		Gateway(const Core::Net::Endpoint& endpoint, const std::string& token, Intent intents);

	private:
		Core::Optional<Core::SharedMutex<Core::Net::Websocket::WSSClient>> mWSS;
		std::unique_ptr<Heartbeat>                                         mHeartbeat;
		std::queue<Core::Net::Websocket::Message>                          mMessageBuffer;
	};
} // namespace Strawberry::Discord::Gateway