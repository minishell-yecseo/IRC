#ifndef PINGCOMMAND_HPP
#define PINGCOMMAND_HPP

#include "Command.hpp"

// ERR_NOTREGISTERED (451)
// ERR_NEEDMOREPARAMS (461)

class PingCommand: public Command {
	public:
		PingCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		std::string	AnyOfError(void);
};
#endif
