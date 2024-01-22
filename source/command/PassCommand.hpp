#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include "Command.hpp"

class PassCommand: public Command {
	public:
		PassCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		std::string	AnyOfError(void);
		bool		CheckClientAuth(void);
};
#endif
