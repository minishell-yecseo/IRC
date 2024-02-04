#include "PartCommand.hpp"

PartCommand::PartCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

void	PartCommand::CheckChannel(const std::string& channel_name) {
	std::map<std::string, Channel> *channel_list;
	std::map<std::string, Channel>::iterator chan;
	std::map<int, char>	chan_member_list;
	int	channel_left_num = 1;

	this->server_->LockChannelListMutex();
	channel_list = &(this->server_->get_channels());
	chan = channel_list->find(channel_name);
	if (chan == channel_list->end()) {
		this->server_->UnlockChannelListMutex();
		this->resp_ = this->resp_ + ERR_NOSUCHCHANNEL + " " + channel_name + " :No such channel";
		return ;
	}
	this->server_->UnlockChannelListMutex();


	this->server_->LockChannelMutex(chan->first);
	if ((chan->second).IsMember(this->client_sock_) == false) {
		this->server_->UnlockChannelMutex(chan->first);
		this->resp_ = this->resp_ + ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel";
	}
	else {
		channel_left_num = (chan->second).Kick(this->client_sock_);
		if (channel_left_num)
			chan_member_list = (chan->second).get_members();
	}
	this->server_->UnlockChannelMutex(chan->first);

	// need seperate here
	std::string sender = this->server_->SearchClientBySock(this->client_sock_);

	if (channel_left_num == 0)
		this->server_->CeaseChannel(channel_name);

	std::string send_message = ":" + sender + " PART " + channel_name;
	if (channel_left_num > 0) {
		std::map<int, char>::const_iterator	iter = chan_member_list.begin();
		SendResponse(iter->first, (send_message + CRLF));
	}
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
		this->resp_ = this->resp_ + ERR_NOTREGISTERED + " :You have not registered";
	else if (this->params_.empty())
		this->resp_ = this->resp_ + ERR_NEEDMOREPARAMS + " PART :Not enough parameters";
	else
		PartEachTarget();
}

void	PartCommand::PartEachTarget(void) {
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
		// need fix
			;
	} catch (std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
