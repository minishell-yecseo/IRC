#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include "Command.hpp"

class PassCommand: public Command {
	protected:
		PassCommand(const std::vector<std::string> &token_list, Server *s, Client *c);

	private:
		void	Run(void);
		void	AnyOfError(void);

		friend class	Request;
};
#endif
