#include "PassCommand.hpp"

PassCommand::PassCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	PassCommand::AnyOfError(void) {
	std::string dummy = this->server_->get_name();
	dummy += ": PASS : ";

	if (this->params_.empty() || this->params_.size() != 1)
		return (dummy + ERR_UNKNOWNERROR + " : parameter number error");

	if (server_->AuthPassword(this->params_[0]) == false)
		return (dummy + ERR_PASSWDMISMATCH);

	dummy = "";
	return dummy;
}

void	PassCommand::Run(void) {
	std::string	error_message = AnyOfError();
	if (error_message.empty() == false) {
		SendResponse(this->client_sock_, error_message);
		DisconnectClient();
		return;
	}

	this->server_->LockClientMutex(this->client_sock_);
	client_->SetAuthFlag(FT_AUTH_PASS);
	this->server_->UnlockClientMutex(this->client_sock_);
	AuthCheckReply();
}
