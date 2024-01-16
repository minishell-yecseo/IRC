#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include <iostream>

#include "Command.hpp"

class NickCommand: public Command {
	public:
		NickCommand(const std::vector<std::string> &token_list);
		void	Run();
}
#endif
