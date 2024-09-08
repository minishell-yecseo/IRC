#include "CapCommand.hpp"

CapCommand::CapCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
}

void	CapCommand::SetInfo(void) {
	this->client_nick_ = SearchClientBySock(this->client_sock_);
}

void	CapCommand::AnyOfError(void) {
	if (this->params_.empty())
		this->resp_ = (std::string)ERR_NEEDMOREPARAMS + " CAP :Not enough parameters";
	else
		this->is_success_ = true;
}

void	CapCommand::Run(void) {
	try {
		AnyOfError();
		if (this->is_success_ == false)
			SendResponse(this->client_sock_, this->resp_.get_format_str());
		else if (this->params_[0].compare("LS") == 0) {
			this->resp_ = (std::string)"CAP * LS :";
			SendResponse(this->client_sock_, this->resp_.get_format_str());
		}
	} catch (std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
