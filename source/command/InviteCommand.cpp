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
	std::string	dummy = chan + nick;
/*
	int	receiver = this->server_->SearchClientByNick(nick);

	if (receiver == FT_INIT_CLIENT_FD)
		return dummy + ERR_NOSUCHNICK + " " + nick + " Invite :No Such User";
	pthread_mutex_lock(&this->server_->pool_->s_channels_mutex_);
	std::map<std::string, Channel>::iterator	it = this->server_->channels_.find(chan);
	if (it == this->server_->channels_.end())
		return dummy + ERR_NOTONCHANNEL + " " + this->params_[1] + " :No such channel";
	if (it.IsMember(this->client_sock_) == false)
		return dummy + ERR_NOTONCHANNEL + " Invite :You're not on that channel";
	if (it.IsOperator(this->client_sock_) == false)
		return dummy + ERR_CHANOPRIVSNEEDED +  ": You're not channel operator";
	if (it.IsMember(receiver) == true)
		return dummy + ERR_USERONCHANNEL + ":  User already in";
	pthread_mutex_unlock(&this->server_->pool_->s_channels_mutex_);
	*/
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

