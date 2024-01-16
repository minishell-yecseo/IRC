#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include <iostream>

#include "Command.hpp"

class UserCommand: public Command {
	public:
		UserCommand(const std::vector<std::string> &token_list);
		void	Run();
}
#endif
