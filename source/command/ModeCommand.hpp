#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

#include <cstring>

#include "Command.hpp"
#include "NumericDefine.hpp"

class ModeCommand: public Command {
	public:
		ModeCommand(const std::vector<std::string> &token_list);
		void		Run(void);
		std::string	AnyOfError(void);
		bool		CheckKeyParam(const std::string& str);
		bool		IsValidMode(const std::string& str);
		std::string	CheckChannel(const std::string& chan);
		bool		IsLimitNumber(const std::string& param);
		void		SetModeInChannel(Channel *c, const std::string& modestr);
		void		ModifyChannel(Channel *c, char mode, bool sing, int *param_index);
		bool		CheckParamCount(const std::string& modestr);
};
#endif
