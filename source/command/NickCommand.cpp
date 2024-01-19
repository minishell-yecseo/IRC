#include "NickCommand.hpp"

NickCommand::NickCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

bool	NickCommand::IsSpecial(char c) {
	if (c == '-' || c == '[' || c == ']' || c == '\\' ||
		c == '`' || c == '^' || c == '{' || c == '}')
		return true;
	return false;
}

bool	NickCommand::IsValidNick(const std::string& str) {
	if (str.empty() == true || isalpha(str[0]) == false)
		return false;
	if (str.size() > 9)
		return false;
	for (size_t i = 0; i < str.size(); ++i) {
		if (i == 0 && (str[i] == '#' || str[i] == ':'))
			return false;
		if (isalpha(str[i]) || isdigit(str[i]) || IsSpecial(str[i]))
			;
		else
			return false;
	}
	return true;
}

void	NickCommand::Run() {
	bool	status = true;
	int		fd;

	std::string message = this->server_->get_name();
	message += ": NICK : ";

	/* baseic ERROR cases checking */
	fd = this->server_->SearchClientByNick(this->params_[0]);
	if (params_.empty() && !(status = false))
		message += ERR_NONICKNAMEGIVEN;
	else if (IsValidNick(this->params_[0]) == false && !(status = false))
		message += ERR_ERRONEUSNICKNAME;
	else if (this->prefix_.empty() == false && IsValidNick(this->prefix_) == false && !(status = false))
		message += ERR_UNKNOWNERROR;
	else if (fd != FT_INIT_CLIENT_FD && !(status = false))
		message += ERR_NICKNAMEINUSE;

	/*error case 1 : prefix exist but not equal to the request client's current name */
	this->server_->pool_->LockClientMutex(this->client_sock_);
	std::string current_nick = client_->get_nick();
	this->server_->pool_->UnlockClientMutex(this->client_sock_);

	if (this->prefix_.empty() == false && this->prefix_.compare(current_nick) != 0) { 
		message += ERR_UNKNOWNERROR;
		message += " : previous nick doesn't match with the prefix";
		status = false;
	}

	/* send message with FAIL cases */
	if (status == false) {
		message += CRLF;
		log::cout << BOLDCYAN << "send message from NickCommand\n" << message << RESET;
		send(this->client_sock_, message.c_str(), message.size(), 0);
		return ;
	}

	/* success case : nick can be changed */
	this->server_->pool_->LockClientMutex(this->client_sock_);
	client_->set_nick(params_[0]);
	this->server_->pool_->UnlockClientMutex(this->client_sock_);

	/* send message with SUCCESS cases */
	message += this->params_[0];
	message += CRLF;
	log::cout << BOLDCYAN << "send message from NickCommand\n" << RED << message << RESET;
	send(this->client_sock_, message.c_str(), message.size(), 0);
}
