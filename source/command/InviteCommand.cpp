#include "InviteCommand.hpp"

InviteCommand::InviteCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

/*
RPL_INVITING (341)
ERR_NEEDMOREPARAMS (461)
ERR_NOSUCHCHANNEL (403)
ERR_NOTONCHANNEL (442)
ERR_CHANOPRIVSNEEDED (482)
ERR_USERONCHANNEL (443)
*/

std::string	InviteCommand::AnyOfError(void) {
	/*
	if (this->params_.size() < 2)
		return ERR_NEEDMOREPARMS;
	if (notinchannel(this->params_[0] == true)
		return ERR_NOTONCHANNEL;
	if (findchannel(this->params_[1] == false)
		return ERR_NOSUCHCHANNEL;
	if (channelmodeornotoper(client) == true)
		return ERR_CHANOPRIVSNEEDED;
	if (finduserinchannel(this->parmas_[1] == true)
		return ERR_USERONCHANNEL;
	*/
	return "";
}

void	InviteCommand::Run() {
	std::string	message;

	message = AnyOfError();
	if (message.empty() == true)
		message = RPL_INVITING;
	//<client> <nick> <channel>
}
