#include "QuitCommand.hpp"

QuitCommand::QuitCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
}

void	QuitCommand::AnyOfError(void) {
    ;
}

void	QuitCommand::Run(void) {
	try {
		Command::DisconnectClient();
		this->resp_ = (std::string)":" + this->server_->SearchClientBySock(this->client_sock_) + " QUIT :Quit: Bye for now!";
		SendResponse(this->client_sock_, this->resp_.get_format_str());
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
