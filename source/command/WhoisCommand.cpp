#include "WhoisCommand.hpp"

WhoisCommand::WhoisCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
}

void	WhoisCommand::AnyOfError(void) {
	if (Command::IsRegistered(this->client_sock_) == false)
		this->resp_ = (std::string)ERR_NOTREGISTERED + " :You have not registered";
	else if (this->params_.empty())
		this->resp_ = (std::string)ERR_NONICKNAMEGIVEN + " :No nickname given";
	else if (SearchClientByNick(this->params_[0]) == INIT_CLIENT_FD)
		this->resp_ = (std::string)ERR_NOSUCHNICK + " " + this->params_[0] +" :No such nick";
	else
		this->is_success_ = true;
}

void	WhoisCommand::Run(void) {
	try {
		AnyOfError();
		if (this->is_success_ == false)
			SendResponse(this->client_sock_, this->resp_.get_format_str());
		else {
			this->resp_ = (std::string)RPL_ENDOFWHOIS + " WHOISEND";
			SendResponse(this->client_sock_, this->resp_.get_format_str());
		}
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
