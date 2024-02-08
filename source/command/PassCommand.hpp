#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include "Command.hpp"

class PassCommand: public Command {
	public:
		PassCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);
};
#endif
