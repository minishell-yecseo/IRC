#include "NickCommand.hpp"

NickCommand::NickCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

bool	NickCommand::IsSpecial(char c) {
	if (c == '-' || c == '[' || c == ']' || c == '\\' ||
		c == '`' || c == '^' || c == '{' || c == '}')
		return true;
	return false;
}

bool	NickCommand::IsValidNick(const std::string& str) {
	if (str.empty() == true || isalpha(str[0]) == false)
		return false;
	if (str.size() > 9)
		return false;
	for (size_t i = 0; i < str.size(); ++i) {
		if (isalpha(str[i]) || isdigit(str[i]) || IsSpecial(str[i]))
			;
		else
			return false;
	}
	return true;
}

void	NickCommand::Run() {
	/*
	if (params_.empty())
		ERR_NONICKNAMEGIVEN;
	else
	{
		if (IsValidNick(this->params_[0]) == false)
			ERR_ERRONEUSNICKNAME;
	}

	if (this->prefix.empty())
	{
		it = find(this->prefix)
		if (it != end())
			ERR_NICKNAMEINUSE
	}
	else if (params.empty() != false)
	{
		it = find(this->params[0]);
		if (it != end())
			ERR_NICKCOLLISION;
	}
	*/
}
