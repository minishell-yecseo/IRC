#ifndef CAPCOMMAND_HPP
#define CAPCOMMAND_HPP

#include "Response.hpp"
#include "Command.hpp"

class CapCommand: public Command {
	public:
		CapCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		std::string	AnyOfError(void);
		std::string	WelcomeBurst(void);
};
#endif
