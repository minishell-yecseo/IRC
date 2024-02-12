#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include "Command.hpp"

class UserCommand: public Command {
	protected:
		UserCommand(const std::vector<std::string> &token_list, Server *s, Client *c);

	private:
		void	Run(void);
		void	AnyOfError(void);
		bool	IsNonwhite(const std::string& str);
		void	SetUserInfo(void);

		friend class	Request;
};
#endif
