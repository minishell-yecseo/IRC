#ifndef PINGCOMMAND_HPP
#define PINGCOMMAND_HPP

#include "Command.hpp"

class PingCommand: public Command {
	public:
		PingCommand(const std::vector<std::string> &token_list);
		void	Run();
};
#endif
