#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

#include "Command.hpp"
#include "NumericDefine.hpp"

class ModeCommand: public Command {
	public:
		ModeCommand(const std::vector<std::string> &token_list);
		void		Run();
		bool		CheckKeyParam(const std::string& str);
		bool		IsValidMode(const std::string& str);
		std::string	AnyOfError(void);
};
#endif
