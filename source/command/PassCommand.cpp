#include "PassCommand.hpp"

PassCommand::PassCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
}

inline bool	CheckClientAuth(Server *server, Client *client, const int& client_sock) {
	bool	status;
	server->LockClientMutex(client_sock);
	status = client->IsAuth(); 
	server->UnlockClientMutex(client_sock);
	return status;
}

void	PassCommand::AnyOfError(void) {
	if (CheckClientAuth(this->server_, this->client_, this->client_sock_))
		this->resp_ = (std::string)ERR_NOTREGISTERED + " PASS :already registered";
	else if (this->params_.empty() || this->params_.size() != 1)
		this->resp_ = (std::string)ERR_UNKNOWNERROR + " :parameter number error";
	else if (server_->AuthPassword(this->params_[0]) == false)
		this->resp_ = (std::string)ERR_PASSWDMISMATCH + " :Password incorrect";
	else
		this->is_success_ = true;
}

void	PassCommand::Run(void) {
	try {
		AnyOfError();
		if (this->is_success_ == false) {
			SendResponse(this->client_sock_, this->resp_.get_format_str());
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

