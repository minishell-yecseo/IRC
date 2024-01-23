#ifndef PRIVMSGCOMMAND_HPP
#define PRIVMSGCOMMAND_HPP

#include "Command.hpp"

/*
ERR_NOSUCHNICK (401) 
ERR_NOSUCHSERVER (402)
ERR_CANNOTSENDTOCHAN (404)
ERR_TOOMANYTARGETS (407)
ERR_NORECIPIENT (411)
ERR_NOTEXTTOSEND (412)
ERR_NOTOPLEVEL (413)
ERR_WILDTOPLEVEL (414)
RPL_AWAY (301)
*/

class PrivmsgCommand: public Command {
	public:
		PrivmsgCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		std::string	AnyOfError(void);
		std::string	CheckTarget(void);
		std::string	UniCast(const std::string& client_name, const std::string& text);
		std::string	BroadCast(const std::string& channel_name, const std::string& text);
};
#endif
