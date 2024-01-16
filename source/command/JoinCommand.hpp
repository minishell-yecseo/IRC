#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

#include <iostream>

#include "Command.hpp"

class JoinCommand: public Command
{
	public:
		JoinCommand(const std::vector<std::string> &token_list);
		void	Run();
}
#endif
