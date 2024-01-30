#include "WhoisCommand.hpp"

WhoisCommand::WhoisCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	WhoisCommand::AnyOfError(void) {
	std::string	dummy;

	if (Command::IsRegistered() == false)
		return dummy + ERR_NOTREGISTERED + " :You have not registered";
	if (this->params_.empty())
		return dummy + ERR_NONICKNAMEGIVEN + " :No nickname given";
	if (this->server_->SearchClientByNick(this->params_[0]) == FT_INIT_CLIENT_FD)
		return dummy + ERR_NOSUCHNICK + " " + this->params_[0] +" :No such nick";
    return dummy;
}

void	WhoisCommand::Run(void) {
	Response	r;

	try {
		r << AnyOfError();
		if (r.IsError() == true)
			return SendResponse(this->client_sock_, r.get_format_str());
		r << RPL_ENDOFWHOIS << " WHOISEND";
		SendResponse(this->client_sock_, r.get_format_str());
		std::string dum = BOLDMAGENTA;
		log::cout << dum + "Whois send: " + r.get_str() + RESET + "\n";
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
