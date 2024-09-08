#include "ModeCommand.hpp"

ModeCommand::ModeCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
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
				int user = SearchClientByNick(this->params_[(*param_index)++]);
				if (user == INIT_CLIENT_FD)
					return ;
				c->Mode(user, '@');
				break ;
			}
			case 't':
				c->set_mode(MODE_TOPIC, true);
				break ;
			case 'k':
				c->set_mode(MODE_KEY, true);
				c->set_key(this->params_[(*param_index)++]);
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
				int user = SearchClientByNick(this->params_[(*param_index)++]);
				if (user == INIT_CLIENT_FD)
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

void	ModeCommand::CheckChannel(const std::string& channel_name) {
	std::map<std::string, Channel*> *channel_list;
	std::map<std::string, Channel*>::iterator chan;

	LockChannelListMutex();
	channel_list = &(get_channels());
	chan = channel_list->find(channel_name);
	if (chan == channel_list->end()) {
		UnlockChannelListMutex();
		this->resp_ = (std::string)ERR_NOSUCHCHANNEL + " " + channel_name + " :No such channel";
		return ;
	}
	UnlockChannelListMutex();

	LockChannelMutex(chan->first);
	if ((chan->second)->IsMember(this->client_sock_) == false)
		this->resp_ = (std::string)ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel";
	else if ((chan->second)->IsOperator(this->client_sock_) == false)
		this->resp_ = (std::string)ERR_CHANOPRIVSNEEDED + " " + channel_name + " :You're not channel operator";
	else {
		this->is_success_ = true;
		SetModeInChannel(chan->second, this->params_[1]);
	}
	UnlockChannelMutex(chan->first);
}

size_t	ModeCommand::CheckParamCount(const std::string& modestr) {
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
			if (modestr[i] == 'o' || modestr[i] == 'k')
				++count;
		}
	}
	return count;
}

void	ModeCommand::AnyOfError(void) {
	if (Command::IsRegistered(this->client_sock_) == false)
		this->resp_ = (std::string)ERR_NOTREGISTERED + " :You have not registered";
	else if (this->params_.empty())
		this->resp_ = (std::string)ERR_NEEDMOREPARAMS + " MODE :Not enough parameters";
	else if ((this->params_[0][0] != '#' && this->params_[0][0] != '&') || this->params_.size() < 2)
		this->resp_ = (std::string)RPL_CHANNELMODEIS + " :Not given modestring";
	else if (IsValidMode(this->params_[1]) == false)
		this->resp_ = (std::string)ERR_UNKNOWNMODE + " " +  this->params_[1] + " :is unknown mode char to me";
	else if (CheckParamCount(this->params_[1]) != this->params_.size() - 2)
		this->resp_ = (std::string)ERR_NEEDMOREPARAMS + " :No match param count";
	else if (CheckKeyParam(this->params_[1]) == false)
		this->resp_ = (std::string)ERR_KEYSET + " :Keyset is unvliad";
	else
		CheckChannel(this->params_[0]);
}

void	ModeCommand::Run(void) {
	try {
		AnyOfError();
		if (this->is_success_ == false)
			SendResponse(this->client_sock_, this->resp_.get_format_str());
		else {
			std::string	sender = SearchClientBySock(this->client_sock_);
			// :sender MODE param param?
			this->resp_ = (std::string)":" + sender + " MODE";
			for (size_t i = 0; i < this->params_.size(); ++i) {
				this->resp_ = this->resp_ + " " + this->params_[i];
			}
			SendResponse(this->client_sock_, this->resp_.get_format_str());
		}
	} catch (std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
