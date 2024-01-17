#ifndef PONGCOMMAND_HPP
#define PONGCOMMAND_HPP

#include "Command.hpp"

class PongCommand: public Command {
	public:
		PongCommand(const std::vector<std::string> &token_list);
		void	Run();
};
#endif
