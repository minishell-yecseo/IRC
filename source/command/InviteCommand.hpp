#ifndef INVITECOMMAND_HPP
#define INVITECOMMAND_HPP

#include "Command.hpp"

class InviteCommand: public Command {
	public:
		InviteCommand(const std::vector<std::string> &token_list);
		void	Run();
};
#endif
