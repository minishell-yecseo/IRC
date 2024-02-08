#include "NickCommand.hpp"

NickCommand::NickCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
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
	if (str[0] == '#' || str[0] == ':')
		return false;
	for (size_t i = 1; i < str.size(); ++i) {
		if (isalpha(str[i]) || isdigit(str[i]) || IsSpecial(str[i]))
			continue;
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
	if (prev_nick.empty() == false && prev_nick.compare(this->sender_nick_) != 0)
		return false;
	return true;
}

void	NickCommand::AnyOfError(void) {
	/* This function checks only about the parameter */
	if (this->params_.empty() == false && IsUniqueNick(this->params_[0]) == false)
		this->resp_ = (std::string)ERR_NICKNAMEINUSE + " " + this->params_[0] + \
		" :Nickname is already in use";
	else if (this->params_.empty() == false && IsValidNick(this->params_[0]) == false)
		this->resp_ = (std::string)ERR_ERRONEUSNICKNAME + " " + this->params_[0] + \
		" :Erroneus nickname";
	else if (this->params_.empty() == true)
		this->resp_ = (std::string)ERR_NONICKNAMEGIVEN + " :No nickname given";
	else
		this->is_success_ = true;
}

void	NickCommand::Run() {
	try {
		sender_nick_ = this->server_->SearchClientBySock(this->client_sock_);
		this->is_registered_ = IsRegistered(this->client_sock_);
		if (this->is_registered_ == true) {
			AuthClientError();
			if (this->is_success_ == false) {
				SendResponse(this->client_sock_, this->resp_.get_format_str());
				return;
			}
		}
		else {
			AnyOfError();
			if (this->is_success_ == false) {
				SendResponse(this->client_sock_, this->resp_.get_format_str());
				DisconnectClient();
				return;
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
		if (this->is_registered_ == true) {
			this->resp_ = (std::string)":" + this->sender_nick_ + " NICK " + this->params_[0];
			SendResponse(this->client_sock_, this->resp_.get_format_str());
		}
	
		/* auth process */
		if (auth_check == false)
			AuthCheckReply();
	} catch (std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}

void	NickCommand::AuthClientError(void) {
	if (this->prefix_.empty() == false && IsValidNick(this->prefix_) == false)
		this->resp_ = (std::string)ERR_UNKNOWNERROR + " " + this->sender_nick_ + " NICK : Wrong prefix name";
	else if (this->prefix_.empty() == false && IsEqualPrevNick(this->prefix_) == false) 
		this->resp_ = (std::string)ERR_UNKNOWNERROR + " " + this->sender_nick_ + " NICK : prefix does not match with previous name";

	if (this->resp_.size() == 0)
		AnyOfError();
}
