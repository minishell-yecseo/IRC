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
    return "";
}

// We just reply ENDOFWHOIS or such ERR
void	WhoisCommand::Run(void) {
	// Need function in mutex
	/*
	if (this->client_->auth_ == false)
		; //ERR_NOTREGISTERD;
	if (this->params_.empty())
		; //ERR_NONICKNAMEGIVEN;
	if (find(this->params_[0]) == end())
		; //ERR_NOSUCHNICK;
	*/
	std::string message = "318 WHOISEND\r\n";
	server_->pool_->LockClientMutex(client_sock_);
	send(client_->get_sock(), message.c_str(), message.size(), 0);
	server_->pool_->UnlockClientMutex(client_sock_);

}
