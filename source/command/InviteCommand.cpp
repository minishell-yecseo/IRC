#include "InviteCommand.hpp"

InviteCommand::InviteCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	InviteCommand::CheckChannel(const std::string& nick, const std::string& channel_name) {
	std::string	dummy;
	int	receiver = this->server_->SearchClientByNick(nick);
	int sender = this->client_sock_;

	if (receiver == FT_INIT_CLIENT_FD)
		return dummy + ERR_NOSUCHNICK + " " + nick + " :No such user";
	std::map<std::string, Channel> channel_list;
	std::map<std::string, Channel>::iterator chan;

	this->server_->LockChannelListMutex();
	channel_list = this->server_->get_channels();
	chan = channel_list.find(channel_name);
	if (chan == channel_list.end()) {
		this->server_->UnlockChannelListMutex();
		return dummy + ERR_NOSUCHCHANNEL + " " + channel_name + " :No such channel.";
	}
	this->server_->UnlockChannelListMutex();

	this->server_->LockChannelMutex(chan->first);
	if ((chan->second).IsMember(sender) == false)
		dummy = dummy + ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel";
	else if ((chan->second).IsOperator(sender) == false)
		dummy = dummy + ERR_CHANOPRIVSNEEDED + " " + channel_name + " :You're not channel operator";
	else if ((chan->second).IsMember(receiver) == true)
		dummy = dummy + ERR_USERONCHANNEL + " " + nick +" " + channel_name + " :is already on cahnnel";
	this->server_->UnlockChannelMutex(chan->first);
	return dummy;
}

std::string	InviteCommand::AnyOfError(void) {
	std::string	dummy;

	if (Command::IsRegistered(this->client_sock_) == false)
		return dummy + ERR_NOTREGISTERED + " :You have not registered";
	if (this->params_.size() < 2)
		return dummy + ERR_NEEDMOREPARAMS + " Invite :Not enough parameters";
	return CheckChannel(this->params_[0], this->params_[1]);
}

void	InviteCommand::Run() {
	Response	r;

	r << AnyOfError();
	if (r.IsError() == true)
		return SendResponse(this->client_sock_, r.get_format_str());
	//<client> <nick> <channel>
	r << RPL_INVITING << " " << this->params_[0] << " " << this->params_[1];
	SendResponse(this->client_sock_, r.get_format_str());
}

