#include "UserCommand.hpp"

UserCommand::UserCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

bool	UserCommand::IsNonwhite(const std::string& str)
{
	for (size_t i = 0; i < this->params_[0].size(); ++i) {
		// NUL WhiteSpace NewLine CarrigeReturn
		if (str[i] == 0 || str[i] == 20 || str[i] == 12 || str[i] == 15)
			return false;
	}
	return true;
}

void	UserCommand::Run() {
/*
	// <username> <hostname> <servername> <realname>
	// realname must be prefix ':' but not irssi
	if (this->params_.size() < 4)
	{
		ERR_NEEDMOREPARAMS (461)
		if (IsNonwhite(this->params_[0]) == false)
			//Not any Numeric error;;;
	}
	// is needed?
	ERR_ALREADYREGISTERED (462)
*/
}
