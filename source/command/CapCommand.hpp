#ifndef CAPCOMMAND_HPP
# define CAPCOMMAND_HPP

#include "Response.hpp"
#include "Command.hpp"

class CapCommand: public Command {
	protected:
		CapCommand(const std::vector<std::string> &token_list, Server *s, Client *c);

	private:
		void	Run(void);
		void	AnyOfError(void);
		void	SetInfo(void);

		std::string	client_nick_;

		friend class	Request;
};
#endif
