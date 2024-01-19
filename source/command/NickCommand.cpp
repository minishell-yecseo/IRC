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

bool	NickCommand::IsUniqueNick(const std::string& nick) {
	int	search_nick_sock;

	search_nick_sock = this->server_->SearchClientByNick(nick);
	if (search_nick_sock == FT_INIT_CLIENT_FD)
		return true;
	return false;
}

bool	NickCommand::IsEqualPrevNick(const std::string& prev_nick) {
	/*error case : prefix exist but not equal to the request client's current name */
	std::string current_nick;

	this->server_->pool_->LockClientMutex(this->client_sock_);//lock
	current_nick = client_->get_nick();
	this->server_->pool_->UnlockClientMutex(this->client_sock_);//unlock

	if (prev_nick.empty() == false && prev_nick.compare(current_nick) != 0)
		return false;
	return true;
}

/*
# define ERR_UNKNOWNERROR		"400"
# define ERR_NONICKNAMEGIVEN	"431"
# define ERR_NICKNAMEINUSE		"433"
# define ERR_ERRONEUSNICKNAME	"432"
*/

std::string	NickCommand::AnyOfError(bool * status) {
	Response	error;

	if (this->prefix_.empty() == false && IsValidNick(this->prefix_) == false)
		error << ERR_UNKNOWNERROR;
	else if (IsEqualPrevNick(this->prefix_) == false) {
		log::cout << "prevNick not equal\n";
		error << ERR_UNKNOWNERROR;
	}
	else if (params_.empty() == false) {
		if (IsUniqueNick(params_[0]) == false)
			error << ERR_NICKNAMEINUSE;
		else if (IsValidNick(params_[0]) == false)
			error << ERR_ERRONEUSNICKNAME;
	} else if (params_.empty() == true)
		error << ERR_NONICKNAMEGIVEN;

	if (error.size() > 0)
		*status = false;

	return error.get_str();
}

void	NickCommand::Run() {
	Response	out;
	bool	status = true;
	
	out << this->server_->get_name() << ": NICK : ";
	std::string	error_message = AnyOfError(&status);

	/* send message with FAIL cases */
	if (status == false) {
		out << error_message;
		log::cout << BOLDCYAN << "send message from NickCommand\n" << out.get_str() << RESET;
		send(this->client_sock_, out.get_chr(), out.size(), 0);
		DisconnectClient();
		return ;
	}

	/* success case : nick can be changed */
	this->server_->pool_->LockClientMutex(this->client_sock_);
	client_->set_nick(params_[0]);
	this->server_->pool_->UnlockClientMutex(this->client_sock_);

	/* send message with SUCCESS cases */
	out << this->params_[0];
	log::cout << BOLDCYAN << "send message from NickCommand\n" << RED << out.get_str() << RESET;
	send(this->client_sock_, out.get_chr(), out.size(), 0);
}
