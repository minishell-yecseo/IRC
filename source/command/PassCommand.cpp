#include "PassCommand.hpp"

inline bool	CheckClientAuth(Server *server, Client *client, const int& client_sock) {
	bool	status;
	server->LockClientMutex(client_sock);
	status = client->IsAuth(); 
	server->UnlockClientMutex(client_sock);
	return status;
}

PassCommand::PassCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	PassCommand::AnyOfError(void) {
	std::string dummy;
	
	if (CheckClientAuth(this->server_, this->client_, this->client_sock_))
			return (dummy + ":" + ERR_UNKNOWNERROR + " PASS : already registered");

	if (this->params_.empty() || this->params_.size() != 1)
		return (dummy + ERR_UNKNOWNERROR + " :parameter number error");

	if (server_->AuthPassword(this->params_[0]) == false)
		return (dummy + ERR_PASSWDMISMATCH) + " :Password incorrect";
	return dummy;
}

void	PassCommand::Run(void) {
	Response	reply;

	try {
		reply << AnyOfError();
		if (reply.IsError() == true) {
			SendResponse(this->client_sock_, reply.get_format_str());
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

