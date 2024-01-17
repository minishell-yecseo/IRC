#ifndef CAPCOMMAND_HPP
#define CAPCOMMAND_HPP

#include "Response.hpp"

class CapCommand: public Command {
	public:
		CapCommand(const std::vector<std::string> &token_list);
		void	Run();
};
#endif
