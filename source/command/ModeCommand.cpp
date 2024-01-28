#include "ModeCommand.hpp"

ModeCommand::ModeCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

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
	if (str[0] == '+' || str[0] == '-')
		;
	else
		return false;
	for (size_t i = 1; i < str.size(); ++i) {
		if (str[i] == 'i' || str[i] == 't' || str[i] == 'k' || str[i] == 'o' || str[i] == 'l' || str[i] == '+' || str[i] == '-')
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
				if (modestr[i] == 'i' || modestr[i] == 't')
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

bool	ModeCommand::IsLimitNumber(char	*mode_list) {
	if (mode_list[4] > 0) {
		std::string&	limit_param = this->params_[mode_list[4]];
		if (limit_param.size() > 9)
			return false;
		for (size_t i = 0; i < limit_param.size(); ++i)
			if (limit_param[i] < '0' || limit_param[i] > '9')
				return false;
	}
	return true;
}

bool	ModeCommand::IsParamEnough(char	*mode_list) {
	size_t	max = 1;

	for (int i = 0; i < 5; ++i) {
		if (mode_list[i] > 0 && max < static_cast<size_t>(mode_list[i]))
			max = static_cast<size_t>(mode_list[i]);
	}
	return (this->params_.size() == max + 1);
}

void	ModeCommand::SetModeInChannel(Channel& c, char* mode_list) {
	// i t k o l
	if (mode_list[0] == -1) 
		c.set_mode(MODE_INVITE, false);
	else if (mode_list[0] > 0) 
		c.set_mode(MODE_INVITE, true);
	if (mode_list[1] == -1) 
		c.set_mode(MODE_TOPIC, false);
	else if (mode_list[1] > 0) {
		c.set_mode(MODE_TOPIC, true);
		c.set_topic(this->params_[mode_list[1]]);
	};
	if (mode_list[2] == -1) 
		c.set_mode(MODE_KEY, false);
	else if (mode_list[2] > 0) {
		c.set_mode(MODE_KEY, true);
		c.set_password(this->params_[mode_list[2]]);
	}
	int	user = this->server_->SearchClientByNick(this->params_[mode_list[3]]);
	if (user == FT_INIT_CLIENT_FD)
		;
	else if (mode_list[3] == -1)
		c.DegradeMember(user);
	else if (mode_list[3] > 0) {
		c.PromoteMember(user);
	}
	if (mode_list[4] == -1) {
		c.set_mode(MODE_LIMIT, false);
		c.unset_limit();
	}
	else if (mode_list[4] > 0) {
		c.set_mode(MODE_LIMIT, true);
		//c.set_limit(params[mode_list[4]])
		c.set_limit(std::atoi(this->params_[mode_list[4]].c_str()));
	}
}

std::string	ModeCommand::CheckChannel(const std::string& channel_name) {
	std::string	dummy;
	std::map<std::string, Channel> *channel_list;
	std::map<std::string, Channel>::iterator chan;

	this->server_->LockChannelListMutex();
	channel_list = &(this->server_->get_channels());
	chan = channel_list->find(channel_name);
	if (chan == channel_list->end()) {
		this->server_->UnlockChannelListMutex();
		return dummy + ERR_NOSUCHCHANNEL + " " + channel_name + " :No such channel.";
	}
	this->server_->UnlockChannelListMutex();

	char*	mode_list = ConvertMode(this->params_[1]);
	
	if (IsParamEnough(mode_list) == false || IsLimitNumber(mode_list) == false) {
		delete[] mode_list;
		return dummy + ERR_NEEDMOREPARAMS + " MODE :Not enough params";
	}

	this->server_->LockChannelMutex(chan->first);
	if ((chan->second).IsMember(this->client_sock_) == false)
		dummy = dummy + ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel.";
	else if ((chan->second).IsOperator(this->client_sock_) == false)
		dummy = dummy + ERR_CHANOPRIVSNEEDED + " " + channel_name + " :You're not channel operator";
	else
		SetModeInChannel(chan->second, mode_list);
	this->server_->UnlockChannelMutex(chan->first);
	delete[] mode_list;
	return dummy;
}

std::string	ModeCommand::AnyOfError(void) {
	std::string	dummy;

	if (this->params_.empty())
		return dummy + ERR_NEEDMOREPARAMS + " :Not enough params";
	if ((this->params_[0][0] != '#' && this->params_[0][0] != '&') || this->params_.size() < 2)
		return dummy + RPL_CHANNELMODEIS + " :Not given modestring";
	if (IsValidMode(this->params_[1]) == false)
		return dummy + ERR_UNKNOWNMODE + " " +  this->params_[1] + " :is unknown mode char to me";
	if (CheckKeyParam(this->params_[1]) == false)
		return dummy + ERR_KEYSET;
	return CheckChannel(this->params_[0]);
}

void	ModeCommand::Run() {
	Response	r;
	
	r << AnyOfError();
	if (r.IsError() == true) {
		log::cout << r.get_str();
		return SendResponse(this->client_sock_, r.get_format_str());
	}
	std::string	sender = this->server_->SearchClientBySock(this->client_sock_);
	r << ":" << sender << " MODE";
	for (size_t i = 0; i < this->params_.size(); ++i) {
		r << " " << this->params_[i];
	}
	log::cout << r.get_str();
	SendResponse(this->client_sock_, r.get_format_str());
}
