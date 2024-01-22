#include "Command.hpp"

Command::~Command(void){
}

bool	Command::IsRegistered(const int&fd) {
	this->server_->pool_->LockClientMutex(fd);
	bool result = this->client_->IsAuth();
	this->server_->pool_->UnlockClientMutex(fd);

	return result;
}

void	Command::SendResponse(const int& sock, const std::string& str) {
	this->server_->LockClientMutex(sock);
	send(sock, str.c_str(), str.size(), 0);
	this->server_->UnlockClientMutex(sock);
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

void	Command::AuthCheckReply(void) {
	Response	auth_message;
	auth_message << RPL_WELCOME;
	bool		auth_status = false;

	this->server_->LockClientMutex(this->client_sock_);//Lock
	auth_status = this->client_->IsAuth();
	this->server_->UnlockClientMutex(this->client_sock_);//Unlock
	
	if (auth_status == true) {
		auth_message << CRLF;
		SendResponse(this->client_sock_, auth_message.get_str());
		log::cout << RED << "send 001\n" << RESET;
	}
}
