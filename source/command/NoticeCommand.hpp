#ifndef NOTICECOMMAND_HPP
#define NOTICECOMMAND_HPP

#include <iostream>

#include "Command.hpp"

class NoticeCommand: public Command {
	public:
		NoticeCommand(const std::vector<std::string> &token_list);
		void	Run();
}
#endif
