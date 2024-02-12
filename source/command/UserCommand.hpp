#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include "Command.hpp"

class UserCommand: public Command {
	private:
		UserCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);
		bool	IsNonwhite(const std::string& str);
		void	SetUserInfo(void);

		friend class	Request;
		friend class	TestUserCommand;
};
#endif
