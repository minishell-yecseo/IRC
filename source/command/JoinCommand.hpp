#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

#include "Command.hpp"
#include "Response.hpp"

class JoinCommand: public Command
{
	public:
		JoinCommand(const std::vector<std::string> &token_list);
		void		Run(void);
		std::string	AnyOfError(void);
		bool		IsChstring(const std::string &str);
};
#endif
