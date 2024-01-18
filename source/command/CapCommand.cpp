#include "CapCommand.hpp"

CapCommand::CapCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

void	CapCommand::Run() {
	/**/
	std::string message;
	message = "CAP * LS :\r\n";

	server_->pool_->LockClientMutex(client_sock_);//Client Lock
	send(client_->get_sock(), message.c_str(), message.size(), 0);
	server_->pool_->UnlockClientMutex(client_sock_);//Client Unlock

	/* server logging */
	log::cout << "(send message) CapCommand->" << YELLOW << message << "\n" << RESET;
}
