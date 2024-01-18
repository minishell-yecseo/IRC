#include "CapCommand.hpp"

CapCommand::CapCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	CapCommand::WelcomeBurst(void)
{
	return 
		"001 <nick> :Welcome to the <network> Network, <nick>[!<user>@<host>]\r\n"
		"002 <nick> :Your host is <servername>, running version <version>\r\n"
		"003 <nick> :This server was created <datetime>\r\n"
		"004 <nick> <servername> <version> <available umodes> <available cmodes> [<cmodes with param>]\r\n"
		"375 <nick> <info>\r\n"
		"376 <nick> :End of /MOTD command\r\n";
}

void	CapCommand::Run() {
	/**/
	std::string message;
	if (this->params_.empty() == true)
		//Invalid CAP command
		;

	if (this->params_[0].compare("LS") == 0)
		message = "CAP * LS :\r\n";
	else if (this->params_[0].compare("END") == 0)
		message = WelcomeBurst();

	server_->pool_->LockClientMutex(client_sock_);//Client Lock
	send(client_->get_sock(), message.c_str(), message.size(), 0);
	server_->pool_->UnlockClientMutex(client_sock_);//Client Unlock

	/* server logging */
	log::cout << "(send message) CapCommand->" << YELLOW << message << "\n" << RESET;
}
