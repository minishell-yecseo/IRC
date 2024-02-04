#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include <ctype.h>

#include "Command.hpp"

class NickCommand: public Command {
	public:
		NickCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		void	AnyOfError(void);
		bool	IsValidNick(const std::string& str);
		bool	IsSpecial(char c);
		bool	IsUniqueNick(const std::string& nick);
		bool	IsEqualPrevNick(const std::string& prev_nick);
		std::string	AuthClientError(void);
	private:
		std::string	sender_nick_;
};

#endif
