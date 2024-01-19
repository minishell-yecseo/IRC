#include "ModeCommand.hpp"

ModeCommand::ModeCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

/*
ERR_NOSUCHCHANNEL (403)
RPL_CHANNELMODEIS (324)
ERR_CHANOPRIVSNEEDED (482)
ERR_NEEDMOREPARAMS
RPL_CHANNELMODEIS
ERR_CHANOPRIVSNEEDED
ERR_NOSUCHNICK
ERR_NOTONCHANNEL
ERR_KEYSET (467)
RPL_BANLIST
RPL_ENDOFBANLIST
ERR_UNKNOWNMODE
ERR_NOSUCHCHANNEL
ERR_USERSDONTMATCH
RPL_UMODEIS
ERR_UMODEUNKNOWNFLAG
*/
bool	ModeCommand::CheckKeyParam(const std::string& str) {
	if (str[0] != '+')
		return true;

	for (size_t i = 1; i < str.size(); ++i) {
		if (str[i] == 'k')
		{
			if (this->params_.size() < 2)
				return false;
			break ;
		}
	}
	return true;
}

bool	ModeCommand::IsValidMode(const std::string& str) {
	if (str[0] != '+' && str[0] != '-')
		return false;
	for (size_t i = 1; i < str.size(); ++i) {
		if (str[i] == 'i' || str[i] == 't' || str[i] == 'k' || str[i] == 'o' || str[i] == 'l')
			;
		else
			return false;
	}
	return true;
}

std::string	ModeCommand::AnyOfError() {
	if (this->params_.empty())
		return ERR_NEEDMOREPARAMS;
	if ((this->params_[0][0] != '#' && this->params_[0][0] != '&') || this->params_.size() < 2)
		return RPL_CHANNELMODEIS;
	if (IsValidMode(this->params_[1]) == false)
		return ERR_UNKNOWNMODE;
	if (CheckKeyParam(this->params_[1]) == false)
		return ERR_KEYSET;
	return "";
}

// i, t, k, o, l
void	ModeCommand::Run() {
	std::string	numeric_reply;
	
	numeric_reply = AnyOfError();
	if (numeric_reply.empty() == false)
		return ; //with numeric reply
	// find channel
	// ERR_NOSUCHCHANNEL (403)
	// check privs
	// ERR_CHANOPRIVSNEEDED
}
