#ifndef INVITECOMMAND_HPP
#define INVITECOMMAND_HPP

#include "Command.hpp"

#define CHANNOTFOUND 	1
#define SENDERNOTFOUND 	2
#define NOTOPERATOR		3
#define	ALREADYIN		4

/*
RPL_INVITING (341)
ERR_NOSUCHNICK (401)
ERR_NEEDMOREPARAMS (461)
ERR_NOSUCHCHANNEL (403)
ERR_NOTONCHANNEL (442)
ERR_CHANOPRIVSNEEDED (482)
ERR_USERONCHANNEL (443)
*/

class InviteCommand: public Command {
	public:
		InviteCommand(const std::vector<std::string> &token_list);
		void		Run(void);
		std::string	AnyOfError(void);
		std::string	CheckChannel(const std::string& nick, const std::string& chan);
};
#endif
