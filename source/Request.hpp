#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>
#include <sstream>
# define INTMAX 2147483647

enum Commands
{
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

class	Request
{
	public:
		Request(std::string request);
		void	SplitRequest(std::string request);
		void	SplitMessage();
		void	ParseRequest(const std::vector<std::string> &token_list);
		std::string	RemoveDuplicateSpace(const std::string &str);
		int		AlphaBaseNumber(const std::string &token);
	protected:
		std::vector<std::string>	message_list_;
};

#endif
