#ifndef NOTICECOMMAND_HPP
#define NOTICECOMMAND_HPP

#include "Command.hpp"

class NoticeCommand: public Command {
	public:
		NoticeCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		std::string	AnyOfError(void);

};
#endif
