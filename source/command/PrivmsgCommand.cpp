#include "PrivmsgCommand.hpp"

PrivmsgCommand::PrivmsgCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string PrivmsgCommand::BroadCast(const std::string& channel_name, const std::string& text) {
	std::string	dummy;
	std::map<std::string, Channel> *channel_list;
	std::map<std::string, Channel>::iterator chan;

	channel_list = &(this->server_->get_channels());
	chan = channel_list->find(channel_name);
	if (chan == channel_list->end())
		return dummy + ERR_NOSUCHCHANNEL + " :No such channel.";

	if ((chan->second).IsMember(this->client_sock_) == false)
		dummy = dummy + ERR_CANNOTSENDTOCHAN + " " + channel_name + " :Can not send to chanel.";
	else if ((chan->second).get_size() < 2)
		dummy = dummy + ERR_NORECIPIENT + " :No recepient given.";
	if (dummy.empty() == false)
		return dummy;

	const std::set<int>	&members = (chan->second).get_members();
	for (std::set<int>::iterator it = members.begin(); it != members.end(); ++it) {
		if (*it == this->client_sock_)
			continue;
		SendResponse(*it, text);
	}
	return dummy;
}

std::string	PrivmsgCommand::UniCast(const std::string& client_name, const std::string& text) {
	std::string	dummy;
	int	sock;

	sock = this->server_->SearchClientByNick(client_name);
	if (sock == FT_INIT_CLIENT_FD) 
		return dummy + ERR_NOSUCHNICK + " :No such nick.";
	SendResponse(sock, text);
	return dummy;
}

std::string	PrivmsgCommand::CheckTarget(void) {
	std::string&	target = this->params_[0];
	std::string	sender = this->server_->SearchClientBySock(this->client_sock_);
	std::string	text = ":" + sender + " PRIVMSG " + target + " :" + this->params_[this->params_.size() - 1] + CRLF;

	if (sender.empty())
		return "Client not found";
	if (target[0] == '#' || target[0] == '&')
		return BroadCast(target, text);
	return UniCast(target, text);
}

std::string	PrivmsgCommand::AnyOfError(void) {
	std::string	dummy;

	if (Command::IsRegistered() == false)
		return dummy + ERR_NOTREGISTERED + " :You have not registered";
	if (this->params_.empty())
		return dummy + ERR_NOTEXTTOSEND + " :No text to send";
	return CheckTarget();
}

void	PrivmsgCommand::Run(void) {
	Response	r;

	try {
		r << AnyOfError();
		if (r.IsError() == true)
			return SendResponse(this->client_sock_, r.get_format_str());
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
