#ifndef PINGCOMMAND_HPP
#define PINGCOMMAND_HPP

#include "Command.hpp"

class PingCommand: public Command {
	private:
		PingCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);

		friend class	Request;
		friend class	TestPingCommand;
};
#endif
