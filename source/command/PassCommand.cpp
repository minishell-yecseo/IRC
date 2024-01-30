#include "PassCommand.hpp"

inline bool	CheckClientAuth(Client *client) {
	bool	status;
	status = client->IsAuth(); 
	return status;
}

PassCommand::PassCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	PassCommand::AnyOfError(void) {
	std::string dummy;
	
	if (CheckClientAuth(this->client_))
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
	
		client_->SetAuthFlag(FT_AUTH_PASS);
		AuthCheckReply();
	} catch (std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}

