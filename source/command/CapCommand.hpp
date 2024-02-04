#ifndef CAPCOMMAND_HPP
#define CAPCOMMAND_HPP

#include "Response.hpp"
#include "Command.hpp"

class CapCommand: public Command {
	public:
		CapCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		void	AnyOfError(void);
		void	SetInfo(void);
	
	private:
		std::string	client_nick_;
};
#endif
