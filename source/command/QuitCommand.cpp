#include "QuitCommand.hpp"

QuitCommand::QuitCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	QuitCommand::AnyOfError(void) {
    return "";
}

//:dan-!d@localhost QUIT :Quit: Bye for now!

void	QuitCommand::Run(void) {
	Response	r;

	try {
		Command::DisconnectClient();
		r << ":" << this->server_->SearchClientBySock(this->client_sock_) << " QUIT :Quit: Bye for now!";
		SendResponse(this->client_sock_, r.get_format_str());
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
