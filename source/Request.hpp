#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>
#include <sstream>

class Command;
class Server;
class Client;

#include "Command.hpp"
#include "log.hpp"

# define INTMAX 2147483647

enum CommandNumbers
{
	CAP = 2070,
	JOIN = 186148,
	KICK = 199509,
	MODE = 238737,
	NICK = 252237,
	PING = 287671,
	PASS = 282405,
	PART = 282380,
	USER = 382088,
	QUIT = 313242,
	TOPIC = 9414213,
	WHOIS = 10661449,
	INVITE = 113723329,
	NOTICE = 173551591,
	PRIVMSG = 866058635,
};

class	Request
{
	private:
		static std::vector<Command *>	*ParseRequest(Server *server, Client *client, std::string request, int *offset);
		static std::vector<std::string>	*SplitRequest(const std::string &request, int *offset);
		static std::vector<Command *>	*SplitMessage(Server *server, Client *client, const std::vector<std::string> &message_list);
		static Command *	CommandFactory(const std::vector<std::string> &token_list, Server *s, Client *c);
		static std::string	RemoveDuplicateSpace(const std::string &str);
		static int		BaseAlphaToNumber(const std::string &token);
		static void	SeperateWhiteSpace(const std::string &str, std::vector<std::string> *token_list);
		static int	SearchCommand(const std::vector<std::string> &token_list);

		friend class	Server;
		friend class	TestRequest;
};

#endif
