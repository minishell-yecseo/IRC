#ifndef UNVALIDCOMMAND_HPP
#define UNVALIDCOMMAND_HPP

#include "Command.hpp"

class UnvalidCommand: public Command {
	private:
		UnvalidCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);

		friend class	Request;
		friend class	TestUnvalidCommand;
};
#endif
