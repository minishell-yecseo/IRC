#ifndef TOPICCOMMAND_HPP
#define TOPICCOMMAND_HPP

#include "Command.hpp"

class TopicCommand: public Command {
	protected:
		TopicCommand(const std::vector<std::string> &token_list, Server *s, Client *c);

	private:
		void	Run(void);
		void	AnyOfError(void);
		void	CheckChannel(const std::string& channel_name, const std::string& topic);
		void	NoticeTopic(Channel* c, const std::string& nick);

		friend class	Request;
};
#endif
