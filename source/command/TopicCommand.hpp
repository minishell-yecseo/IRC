#ifndef TOPICCOMMAND_HPP
#define TOPICCOMMAND_HPP

#include <iostream>

#include "Command.hpp"

class TopicCommand: public Command {
	public:
		TopicCommand(const std::vector<std::string> &token_list);
		void	Run();
}
#endif
