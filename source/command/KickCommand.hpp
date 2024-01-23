#ifndef KICKCOMMAND_HPP
#define KICKCOMMAND_HPP

#include "Command.hpp"

/*
ERR_NEEDMOREPARAMS (461)
ERR_NOSUCHCHANNEL (403)
ERR_CHANOPRIVSNEEDED (482)
ERR_USERNOTINCHANNEL (441)
ERR_NOTONCHANNEL (442)
*/

class KickCommand: public Command {
	public:
		KickCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		std::string	AnyOfError(void);
		std::string	CheckChannel(const std::string& channel_name, const std::string& nick);
};
#endif
