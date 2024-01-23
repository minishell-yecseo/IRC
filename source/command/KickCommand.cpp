#include "KickCommand.hpp"

KickCommand::KickCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

:WiZ!jto@tolsun.oulu.fi KICK #Finnish John

std::string	KickCommand::CheckChannel(const std::string& channel_name, const std::string& nick) {
	std::string	dummy;
	std::map<std::string, Channel>::iterator chan;
	int target = this->server_->SearchClientByNick(nick);

	if (target == FT_INIT_CLIENT_FD)
		return dummy + ERR_NOSUCHNICK + " " + nick + " :No such nick.";
	this->server_->channels_mutex_.lock();
	chan = this->server_->channels_.find(channel_name);
	if (chan == this->server_->channels_.end()) {
		this->server_->channels_mutex_.unlock();
		return dummy + ERR_NOSUCHCHANNEL + " " + channel_name + " :No such channel.";
	}
	this->server_->channels_mutex_.unlock();

	this->server_->LockChannelMutex(chan->first);
	if ((chan->second).IsMember(this->client_sock_) == false)
		dummy = dummy + ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel.";
	else if((chan->second).IsMember(target) == false)
		dummy = dummy + ERR_USERNOTINCHANNEL + " " + nick + " " + channel_name + " :They aren't on the channel";
	else if ((chan->second).IsOperator(this->client_sock_) == false)
		dummy = dummy + ERR_CHANOPRIVSNEEDED + " " + channel_name + " :You're not channel operator";
	else
		(chan->second).Kick(target);
	this->server_->UnlockChannelMutex(chan->first);
	std::string cli = this->server_->SearchClientBySock(this->client_sock_);
	return dummy + ":" + cli + " KICK " + channel_name + " " + nick;
}

std::string	KickCommand::AnyOfError(void) {
	std::string dummy;

	if (this->params_.size() < 2)
		return dummy + ERR_NEEDMOREPARAMS + " :Not enough params";
    return CheckChannel(this->params_[0], this->params_[1]);
}

// NEED any reply? or some job only kick in one channel one client
void	KickCommand::Run(void) {
	Response	r;

	r << AnyOfError();
	SendResponse(this->client_sock_, r.get_format_str());
}
