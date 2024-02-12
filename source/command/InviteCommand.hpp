#ifndef INVITECOMMAND_HPP
#define INVITECOMMAND_HPP

#include "Command.hpp"

class InviteCommand: public Command {
	protected:
		InviteCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		
	private:
		void	Run(void);
		void	AnyOfError(void);
		void	CheckChannel(const std::string& nick, const std::string& chan);
		bool	SetInfo(void);

		std::string	channel_name_;
		std::string receive_nick_;
		std::string	client_nick_;
		int	receiver_;

		friend class	Request;
};
#endif
