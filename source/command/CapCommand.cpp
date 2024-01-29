#include "CapCommand.hpp"

CapCommand::CapCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	CapCommand::AnyOfError(void) {
	std::string	dummy;

	if (this->params_.empty())
		return dummy + ERR_NEEDMOREPARAMS + " Need more param";
	return dummy;
}

void	CapCommand::Run() {
	Response	r;

	try {
		r << AnyOfError();
		if (r.size() > 0)
			return SendResponse(this->client_sock_, r.get_format_str());
		if (this->params_[0].compare("LS") == 0)
			r << "CAP * LS :";
			
		SendResponse(this->client_sock_, r.get_format_str());
	} catch (std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
