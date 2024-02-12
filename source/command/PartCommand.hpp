#ifndef PARTCOMMAND_HPP
#define PARTCOMMAND_HPP

#include "Command.hpp"

class PartCommand: public Command {
	private:
		PartCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);
		void	CheckChannel(const std::string& channel_name);
		void	ParseParam(void);
		void	PartEachTarget(void);
		void	SetInfo(void);
		void	NoticePart(const std::map<int, char>& chan_member_list);

		std::vector<std::string> target_channels_;
		std::string	client_nick_;
		std::string	reason_;

		friend class	Request;
		friend class	TestPartCommand;
};
#endif
