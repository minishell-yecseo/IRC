#include "KickCommand.hpp"

KickCommand::KickCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

// :WiZ!jto@tolsun.oulu.fi KICK #Finnish John

std::string	KickCommand::CheckChannel(const std::string& channel_name, const std::string& nick) {
	std::string	dummy;
	std::map<std::string, Channel> *channel_list;
	std::map<std::string, Channel>::iterator chan;
	int target = this->server_->SearchClientByNick(nick);

	if (target == FT_INIT_CLIENT_FD)
		return dummy + ERR_NOSUCHNICK + " " + nick + " :No such nick.";
	channel_list = &(this->server_->get_channels());
	chan = channel_list->find(channel_name);
	if (chan == channel_list->end())
		return dummy + ERR_NOSUCHCHANNEL + " " + channel_name + " :No such channel.";

	if ((chan->second).IsMember(this->client_sock_) == false)
		dummy = dummy + ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel.";
	else if((chan->second).IsMember(target) == false)
		dummy = dummy + ERR_USERNOTINCHANNEL + " " + nick + " " + channel_name + " :They aren't on the channel";
	else if ((chan->second).IsOperator(this->client_sock_) == false)
		dummy = dummy + ERR_CHANOPRIVSNEEDED + " " + channel_name + " :You're not channel operator";
	else
		(chan->second).Kick(target);
	std::string sender = this->server_->SearchClientBySock(this->client_sock_);
	return dummy + ":" + sender + " KICK " + channel_name + " " + nick;
}

std::string	KickCommand::AnyOfError(void) {
	std::string dummy;

	if (this->params_.size() < 2)
		return dummy + ERR_NEEDMOREPARAMS + " :Not enough params";
    return dummy;
}

// NEED any reply? or some job only kick in one channel one client
void	KickCommand::Run(void) {
	Response	r;

	try {
		r << AnyOfError();
		if (r.IsError() == true)
			return SendResponse(this->client_sock_, r.get_format_str());
		r << CheckChannel(this->params_[0], this->params_[1]);
		SendResponse(this->client_sock_, r.get_format_str());
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
