#ifndef UNVALIDCOMMAND_HPP
#define UNVALIDCOMMAND_HPP

#include "Command.hpp"

class UnvalidCommand: public Command {
	public:
		UnvalidCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		void	AnyOfError(void);
};
#endif
