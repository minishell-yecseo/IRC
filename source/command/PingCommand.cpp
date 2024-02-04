#include "PingCommand.hpp"

PingCommand::PingCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

void	PingCommand::AnyOfError(void) {
	if (Command::IsRegistered(this->client_sock_) == false)
		this->resp_ = this->resp_ + ERR_NOTREGISTERED + " :You have not registered";
	else if (this->params_.empty() || this->params_[0].empty())
		this->resp_ = this->resp_ + ERR_NEEDMOREPARAMS + " PING :Not enough parameters";
	else
		this->is_success_ = true;
}

void	PingCommand::Run(void) {
	try {
		AnyOfError();
		if (this->is_success_ == false)
			SendResponse(this->client_sock_, this->resp_.get_format_str());
		else {
			this->resp_ = this->resp_ + "PONG " + this->params_[0];
			SendResponse(this->client_sock_, this->resp_.get_format_str());
		}
	
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
