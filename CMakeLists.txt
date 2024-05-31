cmake_minimum_required(VERSION 3.25)
project(StrawberryDiscord)
include(FetchContent)


if (PROJECT_IS_TOP_LEVEL)
	FetchContent_Declare(StrawberryConfig
			GIT_REPOSITORY "https://github.com/StrawberryDan/Config.git"
			GIT_TAG "main")
	FetchContent_MakeAvailable(StrawberryConfig)
	include(StrawberryConfig)
endif()


if (NOT TARGET StrawberryCore AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../Core AND IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Core)
	add_subdirectory(../Core ${CMAKE_CURRENT_BINARY_DIR}/Core)
endif ()

if (NOT TARGET StrawberryCodec AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../Codec AND IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Codec)
	add_subdirectory(../Codec ${CMAKE_CURRENT_BINARY_DIR}/Codec)
endif ()

if (NOT TARGET StrawberryNet AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../Net AND IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Net)
	add_subdirectory(../Net ${CMAKE_CURRENT_BINARY_DIR}/Net)
endif ()


add_library(StrawberryDiscord STATIC
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
target_include_directories(StrawberryDiscord PUBLIC src)
target_link_libraries(StrawberryDiscord PUBLIC StrawberryCore StrawberryCodec StrawberryNet)
set_target_properties(StrawberryDiscord PROPERTIES CXX_STANDARD 20)


add_executable(StrawberryDiscordTest
		test/Main.cpp)
target_link_libraries(StrawberryDiscordTest PRIVATE StrawberryDiscord)
set_target_properties(StrawberryDiscordTest PROPERTIES CXX_STANDARD 20)