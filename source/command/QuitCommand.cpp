#include "QuitCommand.hpp"

QuitCommand::QuitCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

void	QuitCommand::AnyOfError(void) {
    ;
}

//:dan-!d@localhost QUIT :Quit: Bye for now!

void	QuitCommand::Run(void) {
	try {
		Command::DisconnectClient();
		this->resp_ = this->resp_ + ":" + this->server_->SearchClientBySock(this->client_sock_) + " QUIT :Quit: Bye for now!";
		SendResponse(this->client_sock_, this->resp_.get_format_str());
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
