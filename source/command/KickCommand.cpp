#include "KickCommand.hpp"

KickCommand::KickCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

void	KickCommand::CheckChannel(const std::string& channel_name, const std::string& nick) {
	std::map<std::string, Channel> *channel_list;
	std::map<std::string, Channel>::iterator chan;
	int target = this->server_->SearchClientByNick(nick);

	if (target == FT_INIT_CLIENT_FD)
		this->resp_ = (std::string)ERR_NOSUCHNICK + " " + nick + " :No such nick.";
	this->server_->LockChannelListMutex();
	channel_list = &(this->server_->get_channels());
	chan = channel_list->find(channel_name);
	if (chan == channel_list->end()) {
		this->server_->UnlockChannelListMutex();
		this->resp_ = (std::string)ERR_NOSUCHCHANNEL + " " + channel_name + " :No such channel.";
		return ;
	}
	this->server_->UnlockChannelListMutex();

	this->server_->LockChannelMutex(chan->first);
	if ((chan->second).IsMember(this->client_sock_) == false)
		this->resp_ = (std::string)ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel.";
	else if((chan->second).IsMember(target) == false)
		this->resp_ = (std::string)ERR_USERNOTINCHANNEL + " " + nick + " " + channel_name + " :They aren't on the channel";
	else if ((chan->second).IsOperator(this->client_sock_) == false)
		this->resp_ = (std::string)ERR_CHANOPRIVSNEEDED + " " + channel_name + " :You're not channel operator";
	else {
		(chan->second).Kick(target);
		this->is_success_ = true;
	}
	this->server_->UnlockChannelMutex(chan->first);
}

void	KickCommand::AnyOfError(void) {
	if (this->params_.size() < 2)
		this->resp_ = (std::string)ERR_NEEDMOREPARAMS + " :Not enough params";
	else
		CheckChannel(this->params_[0], this->params_[1]);
}

// Need SetInfo
void	KickCommand::Run(void) {
	try {
		AnyOfError();
		if (this->is_success_ == false)
			SendResponse(this->client_sock_, this->resp_.get_format_str());
		else{
			std::string sender = this->server_->SearchClientBySock(this->client_sock_);
			this->resp_ = (std::string)":" + sender + " KICK " + channel_name + " " + nick;
			SendResponse(this->client_sock_, this->resp_.get_format_str());
		}
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
