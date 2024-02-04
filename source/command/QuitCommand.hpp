#ifndef QUITCOMMAND_HPP
#define QUITCOMMAND_HPP

#include "Command.hpp"

class QuitCommand: public Command {
	public:
		QuitCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		void	AnyOfError(void);
};
#endif
