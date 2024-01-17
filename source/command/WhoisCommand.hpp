#ifndef WHOISCOMMAND_HPP
#define WHOISCOMMAND_HPP

#include "Response.hpp"
#include "Command.hpp"

class WhoisCommand: public Command {
	public:
		WhoisCommand(const std::vector<std::string> &token_list);
		void	Run();
};
#endif
