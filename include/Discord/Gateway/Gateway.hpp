#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Discord
#include "Discord/Intent.hpp"
#include "Heartbeat.hpp"
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
		using ReceiveResult = Core::Result<Net::Websocket::Message, Net::Websocket::Error>;
		using SendResult = Core::Result<Core::NullType, Net::Websocket::Error>;


	public:
		static Core::Optional<Gateway>
		Connect(const Net::Endpoint& endpoint, const std::string& token, Intent intents);

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
		SendResult Send(const Net::Websocket::Message& msg);


		/// Returns the number of buffered messages in the internal queue.
		size_t BufferedMessageCount();
		/// Stores a message in the internal buffer so that it can be received again by another source.
		void   BufferMessage(Net::Websocket::Message message);

		[[nodiscard]] double GetHeartbeatInterval() const { return mHeartbeat->GetInterval(); }


	private:
		/// Private Constructor
		Gateway(const Net::Endpoint& endpoint, const std::string& token, Intent intents);

	private:
		Core::Optional<Core::SharedMutex<Net::Websocket::WSSClient>> mWSS;
		std::unique_ptr<Heartbeat>                                         mHeartbeat;
		std::queue<Net::Websocket::Message>                          mMessageBuffer;
	};
} // namespace Strawberry::Discord::Gateway
