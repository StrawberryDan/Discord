add_library(Discord STATIC
        include/Discord/Bot.hpp
        src/Bot.cpp
        src/Gateway/Heartbeat.cpp
        include/Discord/Gateway/Heartbeat.hpp
        include/Discord/Gateway/Gateway.hpp
        src/Gateway/Gateway.cpp
        include/Discord/Snowflake.hpp
        src/Snowflake.cpp
        include/Discord/Behaviour.hpp
        include/Discord/Event/GuildCreate.hpp
        include/Discord/Event/Ready.hpp
        src/Events/GuildCreate.cpp
        include/Discord/Error.hpp
        include/Discord/Entity/Guild.hpp
        include/Discord/Entity/Channel.hpp
        src/Entities/Guild.cpp
        src/Entities/Channel.cpp
        include/Discord/EventListener.hpp
        include/Discord/Event/Base.hpp
        src/Voice/Connection.cpp
        include/Discord/Voice/Connection.hpp
        src/Events/Ready.cpp
        include/Discord/Voice/Heartbeat.hpp
        src/Voice/Heartbeat.cpp)
target_include_directories(Discord PUBLIC include)
target_link_libraries(Discord PUBLIC Codec Standard)


add_executable(Discord_Test
        test/Main.cpp)
target_link_libraries(Discord_Test PRIVATE Discord)