#include "Discord/Bot.hpp"



using namespace Strawberry::Discord;



int main(int argc, const char** argv)
{
	Bot bot(argv[1], Intent::GUILDS | Intent::GUILD_MEMBERS | Intent::GUILD_MESSAGES | Intent::MESSAGE_CONTENT);
	bot.Run();
}