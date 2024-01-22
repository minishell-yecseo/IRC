#ifndef WHOISCOMMAND_HPP
#define WHOISCOMMAND_HPP

#include "Response.hpp"
#include "Command.hpp"

/*
 ERR_NOSUCHNICK (401)
 ERR_NOSUCHSERVER (402)
 ERR_NONICKNAMEGIVEN (431)
 RPL_ENDOFWHOIS (318)
 ERR_NOTREGISTERED (451)
*/

/*
 This section describes extension to the common WHOIS command above.
 They exist mainly on historical servers, and are rarely implemented,
 because of resource usage they incur.
 */

class WhoisCommand: public Command {
	public:
		WhoisCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		std::string	AnyOfError(void);
};
#endif
