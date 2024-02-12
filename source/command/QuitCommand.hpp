#ifndef QUITCOMMAND_HPP
#define QUITCOMMAND_HPP

#include "Command.hpp"

class QuitCommand: public Command {
	private:
		QuitCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);

		friend class	Request;
		friend class	TestQuitCommand;
};
#endif
