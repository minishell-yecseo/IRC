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

bool	PassCommand::CheckClientAuth(void) {
	bool	status = true;
	this->server_->LockClientMutex(this->client_sock_);
	this->client_->IsAuth(); 
	this->server_->UnlockClientMutex(this->client_sock_);
	return status;
}

void	PassCommand::Run(void) {
	try {
		if (CheckClientAuth() == false)
			return;
	
		std::string	error_message = AnyOfError();
		if (error_message.empty() == false) {
			error_message += CRLF;
			SendResponse(this->client_sock_, error_message);
			log::cout << "PASS send : " << YELLOW << error_message << RESET << "\n";
			DisconnectClient();
			return;
		}
	
		this->server_->LockClientMutex(this->client_sock_);
		client_->SetAuthFlag(FT_AUTH_PASS);
		this->server_->UnlockClientMutex(this->client_sock_);
		AuthCheckReply();
	} catch (std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
