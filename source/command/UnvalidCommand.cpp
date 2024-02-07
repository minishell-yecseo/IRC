#include "UnvalidCommand.hpp"

UnvalidCommand::UnvalidCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

void	UnvalidCommand::AnyOfError(void) {
	;
}

void	UnvalidCommand::Run() {
	this->resp_ = (std::string)ERR_UNKNOWNCOMMAND + " " + this->command_ + " :Unknown command";
	SendResponse(this->client_sock_, this->resp_.get_format_str());
}
