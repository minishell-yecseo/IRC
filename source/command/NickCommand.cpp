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
	if (params_.empty() == true)
		ERR_NONICKNAMEGIVEN;
	else if (IsValidNick(this->params_[0]) == false)
		ERR_ERRONEUSNICKNAME;
	else if (FindClientByNick(this->params_[0]) != end())
		ERR_NICKNAMEINUSE;
	else if (this->prefix.empty() != false && IsValidNick(this->prefix) == false))
		ERR_UNKNOWNERROR;
	
	it = FindClientByNick(this->prefix_);
	set_client_name(&it, this->params_[0]);
	*/
}
