#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include <ctype.h>

#include "Command.hpp"

class NickCommand: public Command {
	private:
		NickCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);
		bool	IsValidNick(const std::string& str);
		bool	IsSpecial(char c);
		bool	IsUniqueNick(const std::string& nick);
		bool	IsEqualPrevNick(const std::string& prev_nick);
		void	AuthClientError(void);

		std::string	sender_nick_;
		bool		is_registered_;

		friend class	Request;
		friend class	TestNickCommand;
};

#endif
