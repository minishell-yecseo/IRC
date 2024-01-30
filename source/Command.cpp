#include "Command.hpp"

Command::~Command(void){
}

bool	Command::IsRegistered() {
	bool result = this->client_->IsAuth();
	return result;
}

void	Command::SendResponse(const int& sock, const std::string& str) {
	send(sock, str.c_str(), str.size(), 0);
}

Command::Command(const std::vector<std::string> &token_list) {
	size_t	param_index;

	if (token_list[0][0] == ':') {
		this->prefix_ = token_list[0];
		this->command_ = token_list[1];
		param_index = 2;
	} else {
		this->command_ = token_list[0];
		param_index = 1;
	}

	for (size_t index = param_index; index < token_list.size(); ++index) {
		this->params_.push_back(token_list[index]);
	}
}

void	Command::set_server(Server *server) {
	this->server_ = server;
}

void	Command::set_client(Client *client) {
	this->client_ = client;
	this->client_sock_ = client->get_sock();
}

Server*	Command::get_server(void) {
	return server_;
}

Client*	Command::get_client(void) {
	return client_;
}

void	Command::DisconnectClient(void) {
	/* Add to Delete list in Server */
	this->server_->AddDeleteClient(this->client_sock_);
}

 //:server 004 <nick> <servername> <version> <available umodes> <available cmodes> [<cmodes with param>]
void	Command::AuthCheckReply(void) {
	Response	auth_message;

	char	flag;
	bool	auth_status = false;
	bool	send_status = false;

	auth_status = this->client_->IsAuth();
	flag = this->client_->get_auth_flag(FT_AUTH_ALL);
	if (auth_status == false && ((flag & FT_AUTH_ALL) == FT_AUTH_ALL)) {
		send_status = true;
		this->client_->SetAuthFlag(FT_AUTH);
	}
	
	if (send_status) {
		std::string	nick = this->server_->SearchClientBySock(this->client_sock_);
		const std::string& serv_name = this->server_->get_name();
		auth_message << ":" << serv_name << " " << RPL_WELCOME << " " << nick 
			<< " :Welcome to the " << serv_name << " Network, " << nick << CRLF;
		auth_message << ":" << serv_name << " " << RPL_YOURHOST << " " << nick 
			<< " :Your host is  " << serv_name << ", running version " << this->server_->get_version() << CRLF;
		auth_message << ":" << serv_name << " " << RPL_CREATED << " " << nick 
			<< " :This server was create " << this->server_->get_create_time() << CRLF;
		auth_message << ":" << serv_name << " " << RPL_MYINFO << " " << nick 
			<< " " << serv_name << " " << this->server_->get_version() << CRLF;
		SendResponse(this->client_sock_, auth_message.get_str());
	}
}
