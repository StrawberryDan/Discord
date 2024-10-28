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
            src/Discord/Behaviour.hpp
            src/Discord/Bot.hpp
            src/Discord/Entities/Channel.hpp
            src/Discord/Entities/Guild.hpp
            src/Discord/Entities/PermissionOverwrite.hpp
            src/Discord/Error.hpp
            src/Discord/Events/EventBase.hpp
            src/Discord/Events/GuildCreate.hpp
            src/Discord/Events/Ready.hpp
            src/Discord/EventListener.hpp
            src/Discord/Gateway/Gateway.hpp
            src/Discord/Gateway/Heartbeat.hpp
            src/Discord/Snowflake.hpp
            src/Discord/Voice/Connection.hpp
            src/Discord/Voice/Heartbeat.hpp
            src/Discord/Bot.cpp
            src/Discord/Entities/Channel.cpp
            src/Discord/Entities/Guild.cpp
            src/Discord/Entities/PermissionOverwrite.cpp
            src/Discord/EventListener.cpp
            src/Discord/Events/GuildCreate.cpp
            src/Discord/Events/Ready.cpp
            src/Discord/Gateway/Gateway.cpp
            src/Discord/Gateway/Heartbeat.cpp
            src/Discord/Snowflake.cpp
            src/Discord/Voice/Connection.cpp
            src/Discord/Voice/Heartbeat.cpp
            src/Discord/EventListener.cpp)
    new_strawberry_library(NAME StrawberryDiscord SOURCE ${StrawberryDiscord_Source})
    target_include_directories(StrawberryDiscord PUBLIC src)
    target_link_libraries(StrawberryDiscord PUBLIC StrawberryCore StrawberryCodec StrawberryNet)


    new_strawberry_executable(NAME StrawberryDiscordTest
        SOURCE
            test/Main.cpp)
    target_link_libraries(StrawberryDiscordTest PRIVATE StrawberryDiscord)
endif()