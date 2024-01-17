#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include "Command.hpp"

class UserCommand: public Command {
	public:
		UserCommand(const std::vector<std::string> &token_list);
		bool	IsNonwhite(const std::string& str);
		void	Run();
};
#endif
