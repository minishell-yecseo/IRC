#ifndef QUITCOMMAND_HPP
#define QUITCOMMAND_HPP

#include "Command.hpp"

class QuitCommand: public Command {
	protected:
		QuitCommand(const std::vector<std::string> &token_list, Server *s, Client *c);

	private:
		void	Run(void);
		void	AnyOfError(void);

		friend class	Request;
};
#endif
