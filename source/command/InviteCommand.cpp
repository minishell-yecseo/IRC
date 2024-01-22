#include "InviteCommand.hpp"

InviteCommand::InviteCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	InviteCommand::CheckChannel(const std::string& nick, const std::string& chan) {
	std::string	dummy;

	int	receiver = this->server_->SearchClientByNick(nick);
	int sender = 0;
	int	ret = 0;

	sender = this->client_sock_;
	if (receiver == FT_INIT_CLIENT_FD)
		return dummy + ERR_NOSUCHNICK + " " + nick + " :No such user";
	pthread_mutex_lock(&this->server_->pool_->s_channels_mutex_);
	ret = this->server_->CheckInviteError(chan, sender, receiver);
	pthread_mutex_unlock(&this->server_->pool_->s_channels_mutex_);
	if (ret == CHANNOTFOUND)
		return dummy + ERR_NOSUCHCHANNEL + " " + chan + " :No such channel";
	if (ret == SENDERNOTFOUND)
		return dummy + ERR_NOTONCHANNEL + " " + chan + " :You're not on that channel";
	if (ret == NOTOPERATOR)
		return dummy + ERR_CHANOPRIVSNEEDED + " " + chan + " :You're not channel operator";
	if (ret == ALREADYIN)
		return dummy + ERR_USERONCHANNEL + " " + nick +" " + chan + " :is already on cahnnel";
	return dummy;
}

std::string	InviteCommand::AnyOfError(void) {
	std::string	dummy;

	if (this->params_.size() < 2)
		return dummy + ERR_NEEDMOREPARAMS + " Invite :Need more param";
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

