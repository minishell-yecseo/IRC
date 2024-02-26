#ifndef PRIVMSGCOMMAND_HPP
#define PRIVMSGCOMMAND_HPP

#include "Command.hpp"

class PrivmsgCommand: public Command {
	private:
		PrivmsgCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);
		void	CheckTarget(void);
		void	CheckChannel(const std::string& channel_name);
		void	CheckClient(const std::string& client_name);
		void	BroadCast(const std::string& channel_name, Channel *chan);

		friend class	Request;
		friend class	TestPrivmsgCommand;
};
#endif
