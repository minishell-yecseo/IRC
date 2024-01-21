#include "WhoisCommand.hpp"

WhoisCommand::WhoisCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

/*
 *ERR_NOSUCHNICK (401)
 *ERR_NOSUCHSERVER (402)
 *ERR_NONICKNAMEGIVEN (431)
 *RPL_ENDOFWHOIS (318)
 *ERR_NOTREGISTERED (451)
*/

/*
 * This section describes extension to the common WHOIS command above.
 * They exist mainly on historical servers, and are rarely implemented,
 * because of resource usage they incur.
 */

std::string	WhoisCommand::AnyOfError(void) {
	std::string	dummy;

	if (this->client_->IsAuth() == false)
		return dummy + ERR_NOTREGISTERED + " :You have not registered";
	if (this->params_.empty())
		return dummy + ERR_NONICKNAMEGIVEN + " :No nickname given";
	if (this->server_->SearchClientByNick(this->params_[0]) == FT_INIT_CLIENT_FD)
		return dummy + ERR_NOSUCHNICK + " " + this->params_[0] +" :No such nick";
    return dummy;
}

void	WhoisCommand::Run(void) {
	Response	r;

	r << AnyOfError();
	if (r.IsError() == true)
		return SendResponse(this->client_sock_, r.get_format_str());
	r << RPL_ENDOFWHOIS << " WHOISEND";
	SendResponse(this->client_sock_, r.get_format_str());
}
