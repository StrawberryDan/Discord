#include "Discord/Bot.hpp"
#include "Strawberry/Core/Net/Socket/API.hpp"


using namespace Strawberry::Discord;


int main([[maybe_unused]] int argc, const char** argv)
{
	Strawberry::Core::Net::Socket::API::Initialise();
	auto bot = Bot::Connect(argv[1], Intent::GUILDS | Intent::GUILD_MEMBERS | Intent::GUILD_MESSAGES | Intent::MESSAGE_CONTENT);
	bot->Run();
	Strawberry::Core::Net::Socket::API::Terminate();
}