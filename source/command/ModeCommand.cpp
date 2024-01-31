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

bool	ModeCommand::IsLimitNumber(const std::string& param) {
	if (param.size() > 9)
		return false;
	for (size_t i = 0; i < param.size(); ++i) {
		if (param[i] < '0' || param[i] > '9')
			return false;
	}
	return true;
}

void	ModeCommand::ModifyChannel(Channel *c, char mode, bool sign, int *param_index) {
	if (sign == true) {
		switch (mode) {
			case 'i':
				c->set_mode(MODE_INVITE, true);
				break ;
			case 'o': {
				int user = this->server_->SearchClientByNick(this->params_[(*param_index)++]);
				if (user == FT_INIT_CLIENT_FD)
					return ;
				c->Mode(user, '@');
				break ;
			}
			case 't':
				c->set_mode(MODE_TOPIC, true);
				break ;
			case 'k':
				c->set_mode(MODE_KEY, true);
				c->set_password(this->params_[(*param_index)++]);
				break ;
			case 'l': {
				if (IsLimitNumber(this->params_[*param_index]) == true) {
					c->set_mode(MODE_LIMIT, true);
					c->set_limit(std::atoi(this->params_[(*param_index)++].c_str()));
				}
				break ;
			}
		}
	}
	else {
		switch (mode) {
			case 'i':
				c->set_mode(MODE_INVITE, false);
				break ;
			case 'o': {
				int user = this->server_->SearchClientByNick(this->params_[(*param_index)++]);
				if (user == FT_INIT_CLIENT_FD)
					return ;
				c->Mode(user, ' ');
				break ;
			}
			case 't':
				c->set_mode(MODE_TOPIC, false);
				break ;
			case 'k':
				c->set_mode(MODE_KEY, false);
				break ;
			case 'l':
				c->set_mode(MODE_LIMIT, false);
				break ;
		}
	}
}

void	ModeCommand::SetModeInChannel(Channel *c, const std::string& modestr) {
	bool	sign;
	int		param_index = 2;

	for (size_t i = 0; i < modestr.size(); ++i) {
		if (modestr[i] == '+')
			sign = true;
		else if (modestr[i] == '-')
			sign = false;
		else
			ModifyChannel(c, modestr[i], sign, &param_index);
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

	this->server_->LockChannelMutex(chan->first);
	if ((chan->second).IsMember(this->client_sock_) == false)
		dummy = dummy + ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel.";
	else if ((chan->second).IsOperator(this->client_sock_) == false)
		dummy = dummy + ERR_CHANOPRIVSNEEDED + " " + channel_name + " :You're not channel operator";
	else
		SetModeInChannel(&(chan->second), this->params_[1]);
	this->server_->UnlockChannelMutex(chan->first);
	return dummy;
}

bool	ModeCommand::CheckParamCount(const std::string& modestr) {
	bool	sign;
	size_t	count = 0;

	for (size_t i = 0; i < modestr.size(); ++i) {
		if (modestr[i] == '+')
			sign = true;
		else if (modestr[i] == '-')
			sign = false;
		if (sign == true) {
			if (modestr[i] == 'o' || modestr[i] == 'k' || modestr[i] == 'l')
				++count;
		}
		else if (sign == false) {
			if (modestr[i] == 'o')
				++count;
		}
	}
	return (this->params_.size() - 2 == count);
}

std::string	ModeCommand::AnyOfError(void) {
	std::string	dummy;

	if (this->params_.empty())
		return dummy + ERR_NEEDMOREPARAMS + " :Not enough params";
	if ((this->params_[0][0] != '#' && this->params_[0][0] != '&') || this->params_.size() < 2)
		return dummy + RPL_CHANNELMODEIS + " :Not given modestring";
	if (IsValidMode(this->params_[1]) == false)
		return dummy + ERR_UNKNOWNMODE + " " +  this->params_[1] + " :is unknown mode char to me";
	if (CheckParamCount(this->params_[1]) == false)
		return dummy + ERR_NEEDMOREPARAMS + " :No match param count";
	if (CheckKeyParam(this->params_[1]) == false)
		return dummy + ERR_KEYSET + " :Keyset is unvliad";
	return CheckChannel(this->params_[0]);
}

void	ModeCommand::Run() {
	Response	r;

	try {
		r << AnyOfError();
		if (r.IsError() == true)
			return SendResponse(this->client_sock_, r.get_format_str());
		std::string	sender = this->server_->SearchClientBySock(this->client_sock_);
		r << ":" << sender << " MODE";
		for (size_t i = 0; i < this->params_.size(); ++i) {
			r << " " << this->params_[i];
		}
		SendResponse(this->client_sock_, r.get_format_str());
	} catch (std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
