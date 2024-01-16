#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include <iostream>

#include "Command.hpp"

class PassCommand: public Command {
	public:
		PassCommand(const std::vector<std::string> &token_list);
		void	Run();
}
#endif
