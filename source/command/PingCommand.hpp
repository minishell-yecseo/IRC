#ifndef PINGCOMMAND_HPP
#define PINGCOMMAND_HPP

#include "Command.hpp"

// ERR_NOTREGISTERED (451)
// ERR_NEEDMOREPARAMS (461)

class PingCommand: public Command {
	protected:
		PingCommand(const std::vector<std::string> &token_list, Server *s, Client *c);

	private:
		void	Run(void);
		void	AnyOfError(void);
		
		friend class	Request;
};
#endif
