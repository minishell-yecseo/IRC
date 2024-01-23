#include "ModeCommand.hpp"

ModeCommand::ModeCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

/*
RPL_CHANNELMODEIS (324)
ERR_NEEDMOREPARAMS
ERR_KEYSET (467)
ERR_UNKNOWNMODE
ERR_NOSUCHCHANNEL (403)
ERR_CHANOPRIVSNEEDED (482)
ERR_NOTONCHANNEL
RPL_CREATIONTIME (329)

RPL_BANLIST
RPL_ENDOFBANLIST
ERR_NOSUCHNICK
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

int	ModeCommand::ReturnFlagIndex(char c) {
	if (c == 'i')
		return 0;
	if (c == 't')
		return 1;
	if (c == 'k')
		return 2;
	if (c == 'o')
		return 3;
	if (c == 'l')
		return 4;
	return 4;
}

// i t k o l
char	*ModeCommand::ConvertMode(const std::string& modestr) {
	char	*flag = new char[5]();
	bool	sign = true;

	int	order = 2;
	int	index;
	for (size_t i = 0; i < modestr.size(); ++i) {
		if (modestr[i] == '+')
			sign = true;
		else if (modestr[i] == '-')
			sign = false;
		else {
			index = ReturnFlagIndex(modestr[i]);
			if (sign == true) {
				if (modestr[i] == 'i')
					flag[index] = 1;
				else
					flag[index] = order++;
			}
			else
				flag[index] = -1;
		}
	}
	return flag;
}

std::string	ModeCommand::CheckChannel(const std::string& chan) {
	int		ret = 0;
	std::string dummy;

	ret = this->server_->CheckModeError(chan, this->client_sock_);
	if (ret == 1)
		return dummy + ERR_NOSUCHCHANNEL + " " + chan + " :No such channel";
	if (ret == 2)
		return dummy + ERR_NOTONCHANNEL + " " + chan + " :You're not on that channel";
	if (ret == 3)
		return dummy + ERR_CHANOPRIVSNEEDED + " " + chan + " :You're not channel operator";
	return dummy;
}

std::string	ModeCommand::AnyOfError(void) {
	std::string	dummy;

	if (this->params_.empty())
		return dummy + ERR_NEEDMOREPARAMS + " :Not enough params";
	if ((this->params_[0][0] != '#' && this->params_[0][0] != '&') || this->params_.size() < 2)
		return dummy + RPL_CHANNELMODEIS + " :Not given modestring";
	if (IsValidMode(this->params_[1]) == false)
		return dummy + ERR_UNKNOWNMODE;
	if (CheckKeyParam(this->params_[1]) == false)
		return dummy + ERR_KEYSET;
	return CheckChannel(this->params_[0]);
}

bool	ModeCommand::IsParamEnough(char	*mode_list) {
	size_t	max = 0;

	for (int i = 0; i < 5; ++i) {
		if (mode_list[i] > 0 && max < static_cast<size_t>(mode_list[i]))
			max = static_cast<size_t>(mode_list[i]);
	}
	return (this->params_.size() >= max);
}

void	ModeCommand::Run() {
	Response	r;
	
	r << AnyOfError();
	if (r.IsError() == true)
		return SendResponse(this->client_sock_, r.get_format_str());
	char	*mode_list = ConvertMode(this->params_[1]);
	if (IsParamEnough(mode_list) == false) {
		delete[] mode_list;
		r << ERR_NEEDMOREPARAMS << " :Not enough params";
		return SendResponse(this->client_sock_, r.get_format_str());
	}
	this->server_->RunModeInServer(this->params_, mode_list);
	r << RPL_CREATIONTIME << " " << this->params_[0];
	delete[] mode_list;
	log::cout << "Work here\n";
	SendResponse(this->client_sock_, r.get_format_str());
}
