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
	struct kevent	kevent;
	EV_SET(&kevent, this->client_sock_, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	shutdown(this->client_sock_, SHUT_WR);
}
