#ifndef CAPCOMMAND_HPP
# define CAPCOMMAND_HPP

#include "Response.hpp"
#include "Command.hpp"

class CapCommand: public Command {
	private:
		CapCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);
		void	SetInfo(void);

		std::string	client_nick_;

		friend class	Request;
		friend class	TestCapCommand;
};
#endif
