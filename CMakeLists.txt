add_library(Strawberry_Discord STATIC
        include/Discord/Behaviour.hpp
        include/Discord/Bot.hpp
        include/Discord/Entity/Channel.hpp
        include/Discord/Entity/Guild.hpp
        include/Discord/Error.hpp
        include/Discord/Event/EventBase.hpp
        include/Discord/Event/GuildCreate.hpp
        include/Discord/Event/Ready.hpp
        include/Discord/EventListener.hpp
        include/Discord/Gateway/Gateway.hpp
        include/Discord/Gateway/Heartbeat.hpp
        include/Discord/Snowflake.hpp
        include/Discord/Voice/Connection.hpp
        include/Discord/Voice/Heartbeat.hpp
        src/Bot.cpp
        src/Entities/Channel.cpp
        src/Entities/Guild.cpp
        src/Events/GuildCreate.cpp
        src/Events/Ready.cpp
        src/Gateway/Gateway.cpp
        src/Gateway/Heartbeat.cpp
        src/Snowflake.cpp
        src/Voice/Connection.cpp
        src/Voice/Heartbeat.cpp src/EventListener.cpp)
target_include_directories(Strawberry_Discord PUBLIC include)
target_link_libraries(Strawberry_Discord PUBLIC Strawberry_Codec Strawberry_Core)


add_executable(Strawberry_Discord_Test
        test/Main.cpp)
target_link_libraries(Strawberry_Discord_Test PRIVATE Strawberry_Discord)