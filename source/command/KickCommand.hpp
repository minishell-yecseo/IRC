#ifndef KICKCOMMAND_HPP
#define KICKCOMMAND_HPP

#include "Command.hpp"

class KickCommand: public Command {
	public:
		KickCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);
		void	CheckChannel(const std::string& channel_name, const std::string& nick);
		bool	SetInfo(void);
		void	NoticeKick(const std::map<int, char>& chan_member_list);

	private:
		std::string	target_nick_;
		std::string	channel_name_;
		std::string	sender_;
		int	target_;
};
#endif
