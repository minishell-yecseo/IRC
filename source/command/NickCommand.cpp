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

	this->server_->LockClientMutex(this->client_sock_);//lock
	current_nick = client_->get_nick();
	this->server_->UnlockClientMutex(this->client_sock_);//unlock

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

std::string	NickCommand::AnyOfError(void) {
	std::string	dummy;
	/* This function checks only about the parameter */
	if (this->params_.empty() == false) {
		if (IsUniqueNick(this->params_[0]) == false)
			return dummy + ERR_NICKNAMEINUSE + " " + \
				this->sender_nick_ + " " + this->params_[0] + \
				" :Nickname is already in use";
		else if (IsValidNick(this->params_[0]) == false)
			return dummy + ERR_ERRONEUSNICKNAME + " " + \
				this->sender_nick_ + " " + this->params_[0] + \
				" :Erroneus nickname";
	}
	if (this->params_.empty() == true)
		return dummy + ERR_NONICKNAMEGIVEN + " " + this->sender_nick_ + \
			" :No nickname given";
	return "";
}

void	NickCommand::Run() {
	Response	out;
	std::string	error_message;

	sender_nick_ = this->server_->SearchClientBySock(this->client_sock_);
	if (IsRegistered(this->client_sock_) == true) {
		error_message = AuthClientError();
		if (error_message.empty() == false) {
			out << error_message;
			SendResponse(this->client_sock_, out.get_format_str());
			return;
		}
	}
	else {
		error_message = AnyOfError();
		if (error_message.empty() == false) {
			out << error_message;
			SendResponse(this->client_sock_, out.get_format_str());
			DisconnectClient();
		}
	}

	/* success case : nick can be changed */
	bool	auth_check = false;
	this->server_->LockClientMutex(this->client_sock_);
	client_->set_nick(params_[0]);
	client_->SetAuthFlag(FT_AUTH_NICK);
	auth_check = this->client_->IsAuth();
	this->server_->UnlockClientMutex(this->client_sock_);

	/* send message with SUCCESS cases */
	out << params_[0];
	SendResponse(this->client_sock_, out.get_format_str());

	/* auth process */
	if (auth_check == false)
		AuthCheckReply();
}

std::string	NickCommand::AuthClientError(void) {
	std::string	dummy;

	if (this->prefix_.empty() == false && IsValidNick(this->prefix_) == false)
		return dummy + ERR_UNKNOWNERROR + " " + this->sender_nick_ + "NICK : Wrong prefix name";
	else if (this->prefix_.empty() == false && IsEqualPrevNick(this->prefix_) == false) 
		return dummy + ERR_UNKNOWNERROR + " " + this->sender_nick_ + "NICK : prefix does not match with previous name";
	else if (this->prefix_.empty() == true) {
		dummy = AnyOfError();
	}
	return dummy;
}
