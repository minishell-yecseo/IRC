#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

#include <cstring>

#include "Command.hpp"
#include "NumericDefine.hpp"

class ModeCommand: public Command {
	private:
		ModeCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);
		void	CheckChannel(const std::string& chan);
		void	SetModeInChannel(Channel *c, const std::string& modestr);
		void	ModifyChannel(Channel *c, char mode, bool sing, int *param_index);
		bool	CheckKeyParam(const std::string& str);
		bool	IsLimitNumber(const std::string& param);
		bool	IsValidMode(const std::string& str);
		size_t	CheckParamCount(const std::string& modestr);

		friend class	Request;
		friend class	TestModeCommand;
};
#endif
