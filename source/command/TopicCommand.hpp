#ifndef TOPICCOMMAND_HPP
#define TOPICCOMMAND_HPP

#include "Command.hpp"

class TopicCommand: public Command {
	public:
		TopicCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		std::string	AnyOfError(void);
		std::string	CheckChannel(const std::string& channel_name, const std::string& topic);
		void	NoticeTopic(Channel* c, const std::string& topic);
};
#endif
