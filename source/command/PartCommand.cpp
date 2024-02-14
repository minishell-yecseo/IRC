#include "PartCommand.hpp"

PartCommand::PartCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
}

void	PartCommand::SetInfo(void) {
	this->client_nick_ = this->server_->SearchClientBySock(this->client_sock_);
	if (this->params_.size() == 2)
		this->reason_ = this->params_[1];
}

void	PartCommand::NoticePart(const std::map<int, char>& chan_member_list) {
	for (std::map<int, char>::const_iterator it = chan_member_list.begin(); it != chan_member_list.end(); ++it) {
		SendResponse(it->first, this->resp_.get_format_str());
	}
}

void	PartCommand::CheckChannel(const std::string& channel_name) {
	std::map<std::string, Channel*> *channel_list;
	std::map<std::string, Channel*>::iterator chan;
	int	channel_left_num = 1;

	this->server_->LockChannelListMutex();
	channel_list = &(this->server_->get_channels());
	chan = channel_list->find(channel_name);
	if (chan == channel_list->end()) {
		this->server_->UnlockChannelListMutex();
		this->resp_ = (std::string)ERR_NOSUCHCHANNEL + " " + channel_name + " :No such channel";
		return ;
	}
	this->server_->UnlockChannelListMutex();

	this->server_->LockChannelMutex(chan->first);
	if ((chan->second)->IsMember(this->client_sock_) == false) {
		this->server_->UnlockChannelMutex(chan->first);
		this->resp_ = (std::string)ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel";
	}
	else {
		this->resp_ = (std::string)":" + this->client_nick_ + " PART " + channel_name;
		if (this->reason_.empty() == false)
			this->resp_ = this->resp_ + " " + this->reason_;
		channel_left_num = (chan->second)->Kick(this->client_sock_);
		if (channel_left_num > 0)
			NoticePart((chan->second)->get_members());
	}
	this->server_->UnlockChannelMutex(chan->first);

	if (channel_left_num == 0)
		this->server_->CeaseChannel(channel_name);
}

void	PartCommand::ParseParam(void) {
	std::string target = this->params_[0];
	size_t	start = 0, end = 0;
	
	while ((end = target.find(',', start)) != std::string::npos) {
		this->target_channels_.push_back(target.substr(start, end - start));
		start = end + 1;
	}
	this->target_channels_.push_back(target.substr(start));
}

void	PartCommand::AnyOfError(void) {
	if (Command::IsRegistered(this->client_sock_) == false)
		this->resp_ = (std::string)ERR_NOTREGISTERED + " :You have not registered";
	else if (this->params_.empty())
		this->resp_ = (std::string)ERR_NEEDMOREPARAMS + " PART :Not enough parameters";
	else
		this->is_success_ = true;
}

void	PartCommand::PartEachTarget(void) {
	SetInfo();
	ParseParam();
	for (size_t i = 0; i < this->target_channels_.size(); ++i) {
		CheckChannel(this->target_channels_[i]);
		SendResponse(this->client_sock_, this->resp_.get_format_str());
	}
}

void	PartCommand::Run(void) {
	try {
		AnyOfError();
		if (this->is_success_ == false)
			SendResponse(this->client_sock_, this->resp_.get_format_str());
		else
			PartEachTarget();
	} catch (std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
