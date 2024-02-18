#ifndef WHOISCOMMAND_HPP
#define WHOISCOMMAND_HPP

#include "Response.hpp"
#include "Command.hpp"

class WhoisCommand: public Command {
	private:
		WhoisCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);

		friend class	Request;
		friend class	TestWhoisCommand;
};
#endif
