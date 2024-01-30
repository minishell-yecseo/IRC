#include "UserCommand.hpp"

UserCommand::UserCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

void	UserCommand::Run(void) {
	Response	reply;

	try {
		reply << AnyOfError();
		if (reply.IsError()) {
			SendResponse(this->client_sock_, reply.get_format_str());
			return;
		}
		SetUserInfo();
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}

std::string	UserCommand::AnyOfError(void) {
	std::string	dummy;

	if (this->params_.size() < 4)
		return (dummy + ERR_NEEDMOREPARAMS);
	if (IsNonwhite(this->params_[0]) == false)
		return (dummy + ERR_UNKNOWNERROR + " : username must not has whitespace");
	bool isRegistered = this->client_->IsAuth();
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

void	UserCommand::SetUserInfo(void) {
	this->client_->set_user_name(this->params_[0]);
	this->client_->set_host_name(this->params_[1]);
	this->client_->set_server_name(this->params_[2]);
	this->client_->set_real_name(this->params_[3]);
	client_->SetAuthFlag(FT_AUTH_USER);
	AuthCheckReply();
}

