#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>
#include <sstream>

#include "Command.hpp"

# define INTMAX 2147483647

enum CommandNumbers
{
	CAP = 2070,
	JOIN = 186148,
	KICK = 199509,
	NICK = 252237,
	PING = 287671,
	PASS = 282405,
	PARK = 282371,
	PONG = 291727,
	USER = 382088,
	QUIT = 313242,
	TOPIC = 9414213,
	NOTICE = 173551591,
	PRIVMSG = 866058635,
};

class Command;
class	Request
{
	public:
		static std::vector<Command *> ParseRequest(Server *server, Client *client, std::string request, int *offset);
		static int	SplitRequest(const std::string &request, std::vector<std::string> *message_list);
		static void	SplitMessage(Server *server, Client *client, const std::vector<std::string> &message_list, std::vector<Command *> *command_list);
		static Command *	CommandFactory(const std::vector<std::string> &token_list);
		static std::string	RemoveDuplicateSpace(const std::string &str);
		static int		BaseAlphaToNumber(const std::string &token);
		static void	SeperateWhiteSpace(const std::string &str, std::vector<std::string> *token_list);
		static int	SearchCommand(const std::vector<std::string> &token_list);
};

#endif
