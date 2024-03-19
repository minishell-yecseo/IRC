#include "KickCommand.hpp"

KickCommand::KickCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
}

bool	KickCommand::SetInfo(void) {
	this->sender_ = SearchClientBySock(this->client_sock_);
	this->target_ = SearchClientByNick(this->params_[1]);
	if (this->target_ == INIT_CLIENT_FD)
		return false;
	this->target_nick_ = this->params_[1];
	this->channel_name_ = this->params_[0];
	return true;
}

void	KickCommand::NoticeKick(const std::map<int, char>& chan_member_list) {
	for (std::map<int, char>::const_iterator it = chan_member_list.begin(); it != chan_member_list.end(); ++it) {
		SendResponse(it->first, this->resp_.get_format_str());
	}
}

void	KickCommand::Kick(Channel *chan) {
	this->is_success_ = true;
	this->resp_ = (std::string)":" + this->sender_ + " KICK " + this->channel_name_ + " " + this->target_nick_;
	chan->Kick(this->target_);
	NoticeKick(chan->get_members());
	SendResponse(this->target_, this->resp_.get_format_str());
}

void	KickCommand::CheckChannel(const std::string& channel_name, const std::string& nick) {
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
	else if((chan->second)->IsMember(this->target_) == false)
		this->resp_ = (std::string)ERR_USERNOTINCHANNEL + " " + nick + " " + channel_name + " :They aren't on the channel";
	else if ((chan->second)->IsOperator(this->client_sock_) == false)
		this->resp_ = (std::string)ERR_CHANOPRIVSNEEDED + " " + channel_name + " :You're not channel operator";
	else 
		Kick(chan->second);
	UnlockChannelMutex(chan->first);
}

void	KickCommand::AnyOfError(void) {
	if (this->params_.size() < 2)
		this->resp_ = (std::string)ERR_NEEDMOREPARAMS + " KICK :Not enough parameters";
	else if (SetInfo() == false)
		this->resp_ = (std::string)ERR_NOSUCHNICK + " " + this->params_[1] + " :No such nick";
	else
		CheckChannel(this->params_[0], this->params_[1]);
}

void	KickCommand::Run(void) {
	try {
		AnyOfError();
		if (this->is_success_ == false)
			SendResponse(this->client_sock_, this->resp_.get_format_str());
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
