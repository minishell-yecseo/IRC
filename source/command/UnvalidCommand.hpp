#ifndef UNVALIDCOMMAND_HPP
#define UNVALIDCOMMAND_HPP

#include "Command.hpp"

class UnvalidCommand: public Command {
	protected:
		UnvalidCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
	
	private:
		void	Run(void);
		void	AnyOfError(void);

		friend class	Request;
};
#endif
