cmake_minimum_required(VERSION 3.25)
project(StrawberryDiscord)
include(FetchContent)


if (PROJECT_IS_TOP_LEVEL)
    FetchContent_Declare(StrawberryConfig
            GIT_REPOSITORY "https://github.com/StrawberryDan/Config.git"
            GIT_TAG "main")
    FetchContent_MakeAvailable(StrawberryConfig)
    include(StrawberryConfig)
endif ()


if (NOT TARGET StrawberryDiscord)
    find_strawberry_library(NAMES Core Codec Net)

    list(APPEND StrawberryDiscord_Source
        src/Strawberry/Discord/Behaviour.hpp
        src/Strawberry/Discord/Bot.cpp
        src/Strawberry/Discord/Bot.hpp
        src/Strawberry/Discord/Entities/Channel.cpp
        src/Strawberry/Discord/Entities/Channel.hpp
        src/Strawberry/Discord/Entities/Guild.cpp
        src/Strawberry/Discord/Entities/Guild.hpp
        src/Strawberry/Discord/Entities/GuildMember.cpp
        src/Strawberry/Discord/Entities/GuildMember.hpp
        src/Strawberry/Discord/Entities/PermissionOverwrite.cpp
        src/Strawberry/Discord/Entities/PermissionOverwrite.hpp
        src/Strawberry/Discord/Entities/User.cpp
        src/Strawberry/Discord/Entities/User.hpp
        src/Strawberry/Discord/Entities/VoiceState.cpp
        src/Strawberry/Discord/Entities/VoiceState.hpp
        src/Strawberry/Discord/Error.hpp
        src/Strawberry/Discord/Events/GuildCreate.cpp
        src/Strawberry/Discord/Events/GuildCreate.hpp
        src/Strawberry/Discord/Events/MessageCreate.cpp
        src/Strawberry/Discord/Events/MessageCreate.hpp
        src/Strawberry/Discord/Events/Ready.cpp
        src/Strawberry/Discord/Events/Ready.hpp
        src/Strawberry/Discord/Gateway/Gateway.cpp
        src/Strawberry/Discord/Gateway/Gateway.hpp
        src/Strawberry/Discord/Gateway/Heartbeat.cpp
        src/Strawberry/Discord/Gateway/Heartbeat.hpp
        src/Strawberry/Discord/Snowflake.cpp
        src/Strawberry/Discord/Snowflake.hpp
        src/Strawberry/Discord/Voice/Connection.cpp
        src/Strawberry/Discord/Voice/Connection.hpp
        src/Strawberry/Discord/Voice/Heartbeat.cpp
        src/Strawberry/Discord/Voice/Heartbeat.hpp
    )
    new_strawberry_library(NAME StrawberryDiscord SOURCE ${StrawberryDiscord_Source})
    target_include_directories(StrawberryDiscord PUBLIC src)
    target_link_libraries(StrawberryDiscord PUBLIC StrawberryCore StrawberryCodec StrawberryNet)


    new_strawberry_executable(NAME StrawberryDiscordTest
        SOURCE
            test/Main.cpp)
    target_link_libraries(StrawberryDiscordTest PRIVATE StrawberryDiscord)
endif()