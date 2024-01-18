#include "JoinCommand.hpp"

JoinCommand::JoinCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

bool	JoinCommand::IsChstring(const std::string &str) {
	for (size_t i = 0; i < str.size(); ++i) {
		if (str[i] == 0 || str[i] == 7 || str[i] == 20 || str[i] == 12 || str[i] == 15 || str[i] == ',')
			return false;
	}
	return true;
}

/*
ERR_UNKNOWNERROR (400)
ERR_NOSUCHCHANNEL (403)
ERR_BADCHANMASK (476)
ERR_NEEDMOREPARAMS (461)
ERR_BADCHANNELKEY (475)
ERR_INVITEONLYCHAN (473)
*/

std::string	JoinCommand::AnyOfError(const std::string &str) {
	if (str.empty() == true)
		return ERR_NEEDMOREPARAMS;
	if (str[0] != '0' || str[0] != '#' || str[0] != '&')
		return ERR_BADCHANMASK;
	if (str.size() > 200)
		return ERR_NOSUCHCHANNEL;
	if (IsChstring(str) == false)
		return ERR_NOSUCHCHANNEL;
	return "";
}

// It can be empty in params_ because irssi send only 'JOIN'
void	JoinCommand::Run() {
	std::string numeric_reply;
	
	numeric_reply = AnyOfError(this->params_[0]);
	if (numeric_reply.empty() == false)
		return ;
	// Need implement
	// find_channel
	// check mode, check pass
	// if not find make channel with default mode
}
