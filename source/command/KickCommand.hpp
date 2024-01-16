#ifndef KICKCOMMAND_HPP
#define KICKCOMMAND_HPP

#include <iostream>

#include "Command.hpp"

class KickCommand: public Command {
	public:
		KickCommand(const std::vector<std::string> &token_list);
		void	Run();
}
#endif
