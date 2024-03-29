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
		include/Discord/Behaviour.hpp
		include/Discord/Bot.hpp
		include/Discord/Entity/Channel.hpp
		include/Discord/Entity/Guild.hpp
		include/Discord/Entity/PermissionOverwrite.hpp
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
		src/Entities/PermissionOverwrite.cpp
		src/EventListener.cpp
		src/Events/GuildCreate.cpp
		src/Events/Ready.cpp
		src/Gateway/Gateway.cpp
		src/Gateway/Heartbeat.cpp
		src/Snowflake.cpp
		src/Voice/Connection.cpp
		src/Voice/Heartbeat.cpp
		src/EventListener.cpp)
target_include_directories(StrawberryDiscord PUBLIC include)
target_link_libraries(StrawberryDiscord PUBLIC StrawberryCore StrawberryCodec StrawberryNet)
set_target_properties(StrawberryDiscord PROPERTIES CXX_STANDARD 20)


add_executable(StrawberryDiscordTest
		test/Main.cpp)
target_link_libraries(StrawberryDiscordTest PRIVATE StrawberryDiscord)
set_target_properties(StrawberryDiscordTest PROPERTIES CXX_STANDARD 20)