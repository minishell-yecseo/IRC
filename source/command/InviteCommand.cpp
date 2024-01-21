#include "InviteCommand.hpp"

InviteCommand::InviteCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

/*
RPL_INVITING (341)
ERR_NOSUCHNICK (401)
ERR_NEEDMOREPARAMS (461)
ERR_NOSUCHCHANNEL (403)
ERR_NOTONCHANNEL (442)
ERR_CHANOPRIVSNEEDED (482)
ERR_USERONCHANNEL (443)
*/

std::string	InviteCommand::CheckChannel(const std::string& nick, const std::string& chan) {
	std::string	dummy;

	int	receiver = this->server_->SearchClientByNick(nick);
	int	ret = 0;

	if (receiver == FT_INIT_CLIENT_FD)
		return dummy + ERR_NOSUCHNICK + " " + nick + " Invite :No Such User";
	pthread_mutex_lock(&this->server_->pool_->s_channels_mutex_);
	ret = this->server_->CheckInviteError(chan, this->client_sock_, receiver);
	pthread_mutex_unlock(&this->server_->pool_->s_channels_mutex_);
	if (ret == 0)
		return "";
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
	if (r.size() > 0)
		return SendResponse(this->client_sock_, r.get_format_str());
	//<client> <nick> <channel>
}

