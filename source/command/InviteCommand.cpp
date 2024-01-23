#include "InviteCommand.hpp"

InviteCommand::InviteCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	InviteCommand::CheckChannel(const std::string& nick, const std::string& chan) {
	std::string	dummy;

	int	receiver = this->server_->SearchClientByNick(nick);
	int sender = this->client_sock_;
	int	ret = 0;

	if (receiver == FT_INIT_CLIENT_FD)
		return dummy + ERR_NOSUCHNICK + " " + nick + " :No such user";
	ret = this->server_->CheckInviteError(chan, sender, receiver);
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

