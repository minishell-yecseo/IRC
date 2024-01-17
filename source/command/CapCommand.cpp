#include "CapCommand.hpp"

CapCommand::CapCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

void	CapCommand::Run() {
	/**/
	std::string message;
	message = "CAP * LS :\r\n";
	server_->pool_->LockClientMutex(client_sock_);
	write(client_->get_sock(), message.c_str(), message.size());
	server_->pool_->UnlockClientMutex(client_sock_);
}
