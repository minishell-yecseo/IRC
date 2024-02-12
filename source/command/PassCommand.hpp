#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include "Command.hpp"

class PassCommand: public Command {
	private:
		PassCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);

		friend class	Request;
		friend class	TestPassCommand;
};
#endif
