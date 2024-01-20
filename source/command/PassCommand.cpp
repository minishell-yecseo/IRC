#include "PassCommand.hpp"

PassCommand::PassCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	PassCommand::AnyOfError(void) {
    return "";
}

void	PassCommand::Run(void) {
	log::cout << "PASS Command Run\n";
	this->server_->LockClientMutex(this->client_sock_);
	client_->SetAuthFlag(FT_AUTH_PASS);
	this->server_->UnlockClientMutex(this->client_sock_);
	AuthCheckReply();
	/*
	if (this->params_.empty())
	{
		//ERR_NEEDMOREPARAMS
		;
	}
	this->params_[0];
	server_->pool_->LockClientMutex(client_sock_);

	//ERR_ALREADYREGISTERED

	server_->pool_->UnlockClientMutex(client_sock_);
	*/
}
