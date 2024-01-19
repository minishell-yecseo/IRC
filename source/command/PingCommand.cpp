#include "PingCommand.hpp"

PingCommand::PingCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

// ERR_NOTREGISTERED (451)
// ERR_NEEDMOREPARAMS (461)

void	PingCommand::Run() {
	/*
	if (this->client_->auth_ == false)
		; //ERR_NOTREGISTERD;
	if (this->params_.empty() || this->params_[0].empty())
		; //ERR_NEEDMOREPARAMS;
	*/
	// Need get_server_name function
	std::string	message = "PONG " + this->params_[0] + "\r\n";
	log::cout << message;
	server_->pool_->LockClientMutex(client_sock_);
	send(client_->get_sock(), message.c_str(), message.size(), 0);
	server_->pool_->UnlockClientMutex(client_sock_);
}
