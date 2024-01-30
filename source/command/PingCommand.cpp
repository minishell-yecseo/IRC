#include "PingCommand.hpp"

PingCommand::PingCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	PingCommand::AnyOfError(void) {
	std::string	dummy;

	if (Command::IsRegistered() == false)
		return dummy + ERR_NOTREGISTERED + " :You have not registered";
	if (this->params_.empty() || this->params_[0].empty())
		return dummy + ERR_NEEDMOREPARAMS + " PING :Not enough parameters";
    return dummy;
}

void	PingCommand::Run(void) {
	Response	r;

	try {
		r << AnyOfError();
		if (r.IsError() == true)
			return SendResponse(this->client_sock_, r.get_format_str());
	
		r << "PONG " << this->params_[0] ;
		SendResponse(this->client_sock_, r.get_format_str());
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
