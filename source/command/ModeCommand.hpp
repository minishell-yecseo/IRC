#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

#include "Command.hpp"

class ModeCommand: public Command {
	public:
		ModeCommand(const std::vector<std::string> &token_list);
		void	Run();
};
#endif
