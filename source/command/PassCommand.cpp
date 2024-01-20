#include "PassCommand.hpp"

PassCommand::PassCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	PassCommand::AnyOfError(void) {
    return "";
}

void	PassCommand::Run(void) {
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
