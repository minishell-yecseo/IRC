#include "Command.hpp"

Command::~Command(void){
}

bool	Command::IsRegistered(const int&fd) {
	this->server_->LockClientMutex(fd);
	bool result = this->client_->IsAuth();
	this->server_->UnlockClientMutex(fd);

	return result;
}

void	Command::SendResponse(const int& sock, const std::string& str) {
	this->server_->LockClientMutex(sock);
	send(sock, str.c_str(), str.size(), 0);
	this->server_->UnlockClientMutex(sock);
}

Command::Command(const std::vector<std::string> &token_list, Server *s, Client *c) {
	size_t	param_index;

	if (token_list[0][0] == ':') {
		this->prefix_ = token_list[0].substr(1);
		if (token_list.size() > 1)
			this->command_ = token_list[1];
		param_index = 2;
	} else {
		this->command_ = token_list[0];
		param_index = 1;
	}

	for (size_t index = param_index; index < token_list.size(); ++index) {
		this->params_.push_back(token_list[index]);
	}
	this->is_success_ = false;
	this->server_ = s;
	this->client_ = c;
	this->client_sock_ = c->get_sock();
}

void	Command::DisconnectClient(void) {
	/* Add to Delete list in Server */
	this->server_->AddDeleteClient(this->client_sock_);
}

void	Command::AuthCheckReply(void) {
	Response	auth_message;

	char	flag;
	bool	auth_status = false;
	bool	send_status = false;

	this->server_->LockClientMutex(this->client_sock_);
	auth_status = this->client_->IsAuth();
	flag = this->client_->get_auth_flag(FT_AUTH_ALL);
	if (auth_status == false && ((flag & FT_AUTH_ALL) == FT_AUTH_ALL)) {
		send_status = true;
		this->client_->SetAuthFlag(FT_AUTH);
	}
	this->server_->UnlockClientMutex(this->client_sock_);//Unlock
	
	if (send_status) {
		std::string	nick = this->server_->SearchClientBySock(this->client_sock_);
		const std::string& serv_name = this->server_->get_name();
		auth_message << ":" << serv_name << " " << RPL_WELCOME << " " << nick 
			<< " :Welcome to the " << serv_name << " Network, " << nick << CRLF;
		auth_message << ":" << serv_name << " " << RPL_YOURHOST << " " << nick 
			<< " :Your host is " << serv_name << ", running version " << this->server_->get_version() << CRLF;
		auth_message << ":" << serv_name << " " << RPL_CREATED << " " << nick 
			<< " :This server was create " << this->server_->get_create_time() << CRLF;
		auth_message << ":" << serv_name << " " << RPL_MYINFO << " " << nick 
			<< " " << serv_name << " " << this->server_->get_version() << CRLF;
		SendResponse(this->client_sock_, auth_message.get_str());
	}
}

const std::string&	Command::get_response(void) {
	return this->resp_.get_str();
}