#include "PrivmsgCommand.hpp"

PrivmsgCommand::PrivmsgCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
}

void PrivmsgCommand::BroadCast(const std::string& channel_name) {
	std::map<std::string, Channel*> *channel_list;
	std::map<std::string, Channel*>::iterator chan;

	this->server_->LockChannelListMutex();
	channel_list = &(this->server_->get_channels());
	chan = channel_list->find(channel_name);
	if (chan == channel_list->end()) {
		this->server_->UnlockChannelListMutex();
		this->resp_ = (std::string)ERR_NOSUCHCHANNEL + " :No such channel";
		return ;
	}
	this->server_->UnlockChannelListMutex();

	this->server_->LockChannelMutex(chan->first);
	if ((chan->second)->IsMember(this->client_sock_) == false)
		this->resp_ = (std::string)ERR_CANNOTSENDTOCHAN + " " + channel_name + " :Can not send to chanel";
	else if ((chan->second)->get_size() < 2)
		this->resp_ = (std::string)ERR_NORECIPIENT + " :No recepient given";
	else {
		const std::map<int, char>	&members = (chan->second)->get_members();
		this->is_success_ = true;
		for (std::map<int, char>::const_iterator it = members.begin(); it != members.end(); ++it) {
			if (it->first == this->client_sock_)
				continue;
			SendResponse(it->first, this->resp_.get_format_str());
		}
	}
	this->server_->UnlockChannelMutex(chan->first);
}

void	PrivmsgCommand::UniCast(const std::string& client_name) {
	int	sock;

	sock = this->server_->SearchClientByNick(client_name);
	if (sock == FT_INIT_CLIENT_FD) 
		this->resp_ = (std::string)ERR_NOSUCHNICK + " :No such nick";
	else {
		SendResponse(sock, this->resp_.get_format_str());
		this->is_success_ = true;
	}
}

void	PrivmsgCommand::CheckTarget(void) {
	std::string	&target = this->params_[0];
	std::string	sender = this->server_->SearchClientBySock(this->client_sock_);
	this->resp_ = ":" + sender + " PRIVMSG " + target + " :" + this->params_[this->params_.size() - 1];

	if (sender.empty())
		this->resp_ = (std::string)"Client not found";
	else if (target[0] == '#' || target[0] == '&')
		BroadCast(target);
	else 
		UniCast(target);
}

void	PrivmsgCommand::AnyOfError(void) {
	if (Command::IsRegistered(this->client_sock_) == false)
		this->resp_ = (std::string)ERR_NOTREGISTERED + " :You have not registered";
	else if (this->params_.empty())
		this->resp_ = (std::string)ERR_NOTEXTTOSEND + " :No text to send";
	else
		CheckTarget();
}

void	PrivmsgCommand::Run(void) {
	try {
		AnyOfError();
		if (this->is_success_ == false) {
			SendResponse(this->client_sock_, this->resp_.get_format_str());
			return;
		}
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
