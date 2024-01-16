#ifndef PARTCOMMAND_HPP
#define PARTCOMMAND_HPP

#include <iostream>

#include "Command.hpp"

class PartCommand: public Command {
	public:
		PartCommand(const std::vector<std::string> &token_list);
		void	Run();
}
#endif
