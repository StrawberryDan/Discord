#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Codec
#include "Codec/Audio/Mixer.hpp"
#include "Codec/Packet.hpp"
#include "Codec/SodiumEncrypter.hpp"
// Discord
#include "Strawberry/Discord/Gateway/Gateway.hpp"
#include "Strawberry/Discord/Snowflake.hpp"
#include "Strawberry/Discord/Voice/VoiceConnectionHeartbeat.hpp"
// Core
#include "Strawberry/Net/Socket/UDPSocket.hpp"
#include "Strawberry/Net/Websocket/WebsocketClient.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Thread/RepeatingTask.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <Codec/Audio/AudioEncoder.hpp>
#include <string>

namespace Strawberry::Discord::Voice
{
    /// Class for representing a voice connection for a discord bot.
    class Connection
    {
        public:
            Connection(Core::SharedMutex<Gateway::Gateway> gateway, const std::string& sessionId, Snowflake guildId, Snowflake channelId, Snowflake userId);

            /// Only move construction allowed.
            Connection(const Connection&)            = delete;
            Connection(Connection&&)                 = delete;
            Connection& operator=(const Connection&) = delete;
            Connection& operator=(Connection&&)      = delete;
            ~Connection();


            bool IsOk() const
            {
                return mGateway.Lock()->IsOk();
            }


            std::shared_ptr<Codec::Audio::Mixer::InputChannel> CreateInputChannel();


            Snowflake GetGuild() const
            {
                return mGuild;
            }


            Snowflake GetChannel() const
            {
                return mChannel;
            }


            Snowflake GetUser() const
            {
                return mUser;
            }

        protected:
            void SetSpeaking(bool speaking);

        private:
            // Key is a 32 byte date slice
            using Key = std::array<uint8_t, 32>;

        private:
            /// Connections
            Core::SharedMutex<Gateway::Gateway>          mGateway;
            Core::SharedMutex<Net::Websocket::WSSClient> mVoiceWSS;
            Core::Optional<VoiceConnectionHeartbeat>                    mVoiceWSSHeartbeat;
            Core::Optional<Net::Endpoint>                mUDPVoiceEndpoint;
            Core::Optional<Net::Socket::UDPSocket>       mUDPVoiceConnection;
            Core::Optional<Codec::SodiumEncrypter>       mSodiumEncrypter;


            /// Current Voice Channels and User
            const Snowflake mGuild;
            const Snowflake mChannel;
            const Snowflake mUser;

            /// SSRC for our RTP Connection
            Core::Optional<uint32_t> mSSRC;

            /// Whether the client is considered to be speaking. Corresponds to the speaking messaged in Discord API.
            bool mIsSpeaking = false;

            /// Voice Packet Buffer
            Codec::Audio::Mixer   mAudioMixer;
            Codec::Audio::AudioEncoder mOpusEncoder;



            Core::Optional<Core::RepeatingTask> mVoiceSendingThread;
            std::chrono::steady_clock mClock;
            std::chrono::steady_clock::time_point mNextSendTime = mClock.now();
            uint32_t                            mLastSequenceNumber = 0;
            uint32_t                            mLastTimestamp      = 0;
    };
} // namespace Strawberry::Discord::Voice
