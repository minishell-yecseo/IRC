#ifndef WHOISCOMMAND_HPP
#define WHOISCOMMAND_HPP

#include "Response.hpp"
#include "Command.hpp"

/*
 This section describes extension to the common WHOIS command above.
 They exist mainly on historical servers, and are rarely implemented,
 because of resource usage they incur.
 */

class WhoisCommand: public Command {
	private:
		WhoisCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);

		friend class	Request;
};
#endif
