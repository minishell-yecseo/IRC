#include "UserCommand.hpp"

UserCommand::UserCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
}

void	UserCommand::Run(void) {
	try {
		AnyOfError();
		if (this->is_success_ == false) {
			SendResponse(this->client_sock_, this->resp_.get_format_str());
			return;
		}
		SetUserInfo();
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}

void	UserCommand::AnyOfError(void) {
	if (this->params_.size() < 4) {
		this->resp_ = (std::string)ERR_NEEDMOREPARAMS + " USER :Not enough parameters";
		return;
	}
	if (IsNonwhite(this->params_[0]) == false) {
		this->resp_ = (std::string)ERR_UNKNOWNERROR + " :username must not has whitespace";
		return;
	}
	this->server_->LockClientMutex(this->client_sock_);
	bool isRegistered = this->client_->IsAuth();
	this->server_->UnlockClientMutex(this->client_sock_);
	if (isRegistered == true) {
		this->resp_ = (std::string)ERR_ALREADYREGISTERED + " USER :already registered";
		return;
	}
	this->is_success_ = true;
}

bool	UserCommand::IsNonwhite(const std::string& str) {
	for (size_t i = 0; i < str.size(); ++i) {
		if (isspace(str[i]) != 0 || str[i] == 0)
			return false;
	}
	return true;
}

void	UserCommand::SetUserInfo(void) {
	this->server_->LockClientMutex(this->client_sock_);
	this->client_->set_user_name(this->params_[0]);
	this->client_->set_host_name(this->params_[1]);
	this->client_->set_server_name(this->params_[2]);
	this->client_->set_real_name(this->params_[3]);
	client_->SetAuthFlag(FT_AUTH_USER);
	this->server_->UnlockClientMutex(this->client_sock_);
	AuthCheckReply();
}

