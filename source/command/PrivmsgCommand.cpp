#include "PrivmsgCommand.hpp"

PrivmsgCommand::PrivmsgCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string PrivmsgCommand::BroadCast(const std::string& channel_name, const std::string& text) {
	std::string	dummy;
	std::map<std::string, Channel>::iterator chan;

	this->server_->channels_mutex_.lock();
	chan = this->server_->channels_.find(channel_name);
	if (chan == this->server_->channels_.end()) {
		this->server_->channels_mutex_.unlock();
		return dummy + ERR_NOSUCHCHANNEL + " :No such channel.";
	}
	this->server_->channels_mutex_.unlock();

	this->server_->LockChannelMutex(chan->first);
	if ((chan->second).IsMember(this->client_sock_) == false)
		dummy = dummy + ERR_CANNOTSENDTOCHAN + " " + channel_name + " :Can not send to chanel.";
	else if ((chan->second).get_size() < 2)
		dummy = dummy + ERR_NORECIPIENT + " :No recepient given.";
	this->server_->UnlockChannelMutex(chan->first);
	if (dummy.empty() == false)
		return dummy;

	std::map<int, std::string>	members;
	this->server_->get_channel_members(members, channel_name, FT_CH_MEMBER);
	this->server_->LockChannelMutex(chan->first);
	for (std::map<int, std::string>::iterator it = members.begin(); it != members.end(); ++it) {
		SendResponse(it->first, text);
	}
	this->server_->UnlockChannelMutex(chan->first);
	return dummy;
}

std::string	PrivmsgCommand::UniCast(const std::string& client_name, const std::string& text) {
	std::string	dummy;
	int	sock;

	this->server_->clients_mutex_.lock();
	sock = this->server_->SearchClientByNick(client_name);
	if (sock == FT_INIT_CLIENT_FD) {
		this->server_->clients_mutex_.unlock();
		return dummy + ERR_NOSUCHNICK + " :No such nick.";
	}
	this->server_->clients_mutex_.unlock();

	SendResponse(sock, text);
	return dummy;
}

std::string	PrivmsgCommand::CheckTarget(void) {
	std::string&	target = this->params_[0];
	std::string	sender = this->server_->SearchClientBySock(this->client_sock_);
	std::string	text = sender + ": " + this->params_[this->params_.size() - 1] + CRLF;

	if (sender.empty())
		return "Client not found";
	if (target[0] == '#' || target[0] == '&')
		return BroadCast(target, text);
	return UniCast(target, text);
}

std::string	PrivmsgCommand::AnyOfError(void) {
	std::string	dummy;

	if (Command::IsRegistered(this->client_sock_) == false)
		return dummy + ERR_NOTREGISTERED + " :You have not registered";
	if (this->params_.empty())
		return dummy + ERR_NOTEXTTOSEND + " :No text to send";
	return dummy;
}

void	PrivmsgCommand::Run(void) {
	Response	r;

	r << AnyOfError();
	if (r.IsError() == true)
		return SendResponse(this->client_sock_, r.get_format_str());
	r << CheckTarget();
	if (r.IsError() == true)
		return SendResponse(this->client_sock_, r.get_format_str());
}
