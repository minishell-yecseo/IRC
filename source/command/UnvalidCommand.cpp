#include "UnvalidCommand.hpp"

UnvalidCommand::UnvalidCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
}

void	UnvalidCommand::AnyOfError(void) {
	;
}

void	UnvalidCommand::Run(void) {
	this->resp_ = (std::string)ERR_UNKNOWNCOMMAND + " " + this->command_ + " :Unknown command";
	SendResponse(this->client_sock_, this->resp_.get_format_str());
}
