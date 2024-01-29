#include "UserCommand.hpp"

UserCommand::UserCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	UserCommand::AnyOfError(void) {
	std::string	dummy;

	if (this->params_.size() < 4)
		return (dummy + ERR_NEEDMOREPARAMS);
	if (IsNonwhite(this->params_[0]) == false)
		return (dummy + ERR_UNKNOWNERROR + " : username must not has whitespace");
	this->server_->LockClientMutex(this->client_sock_);
	bool isRegistered = this->client_->IsAuth();
	this->server_->UnlockClientMutex(this->client_sock_);
	if (isRegistered == true)
		return (dummy + ERR_ALREADYREGISTERED);
    return "";
}

bool	UserCommand::IsNonwhite(const std::string& str) {
	for (size_t i = 0; i < str.size(); ++i) {
		// NUL WhiteSpace NewLine CarrigeReturn
		if (str[i] == 0 || str[i] == 20 || str[i] == 12 || str[i] == 15)
			return false;
	}
	return true;
}

void	UserCommand::Run(void) {
	Response	reply;

	try {
		// <username> <hostname> <servername> <realname>
		// realname must be prefix ':' but not irssi
		reply << AnyOfError();
		if (reply.IsError()) {
			SendResponse(this->client_sock_, reply.get_format_str());
			return;
		}
	
		this->server_->LockClientMutex(this->client_sock_);
		client_->SetAuthFlag(FT_AUTH_USER);
		this->server_->UnlockClientMutex(this->client_sock_);
		AuthCheckReply();
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
