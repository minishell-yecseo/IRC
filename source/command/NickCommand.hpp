#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include <ctype.h>

#include "Command.hpp"

class NickCommand: public Command {
	public:
		NickCommand(const std::vector<std::string> &token_list);
		void	Run();
		bool	IsValidNick(const std::string& str);
		bool	IsSpecial(char c);
};
#endif
