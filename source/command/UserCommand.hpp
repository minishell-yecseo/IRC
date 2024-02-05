#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include "Command.hpp"

class UserCommand: public Command {
	public:
		UserCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		void	AnyOfError(void);

	private:
		bool	IsNonwhite(const std::string& str);
		void	SetUserInfo(void);
};
#endif
