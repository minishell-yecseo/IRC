#include "Command.hpp"

Command::Command(const std::vector<std::string> &token_list, ServerInterface *s, Client *c) {
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

Command::~Command(void){
}

bool	Command::IsRegistered(const int&fd) {
	LockClientMutex(fd);
	bool result = this->client_->IsAuth();
	UnlockClientMutex(fd);

	return result;
}

void	Command::SendResponse(const int& sock, const std::string& str) {
	LockClientMutex(sock);
	send(sock, str.c_str(), str.size(), 0);
	UnlockClientMutex(sock);
}

void	Command::DisconnectClient(void) {
	/* Add to Delete list in Server */
	AddDeleteClient(this->client_sock_);
}

void	Command::AuthCheckReply(void) {
	Response	auth_message;

	char	flag;
	bool	auth_status = false;
	bool	send_status = false;

	LockClientMutex(this->client_sock_);
	auth_status = this->client_->IsAuth();
	flag = this->client_->get_auth_flag(AUTH_ALL);
	if (auth_status == false && ((flag & AUTH_ALL) == AUTH_ALL)) {
		send_status = true;
		this->client_->SetAuthFlag(AUTH);
	}
	UnlockClientMutex(this->client_sock_);//Unlock
	
	if (send_status) {
		std::string	nick = SearchClientBySock(this->client_sock_);
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

bool Command::LockClientMutex(const int& sock) {
	return this->server_->LockClientMutex(sock);
}

void Command::UnlockClientMutex(const int& sock) {
	this->server_->UnlockClientMutex(sock);
}

bool	Command::AddChannelMutex(const std::string& name) {
	return this->server_->AddChannelMutex(name);
}

bool	Command::DeleteChannelMutex(const std::string& name) {
	return this->server_->DeleteChannelMutex(name);
}

bool	Command::LockChannelMutex(const std::string& name) {
	return this->server_->LockChannelMutex(name);
}

void	Command::UnlockChannelMutex(const std::string& name) {
	this->server_->UnlockChannelMutex(name);
}

bool	Command::AuthPassword(const std::string& password) {
	return this->server_->AuthPassword(password);
}

bool	Command::LockChannelListMutex(void) {
	return this->server_->LockChannelListMutex();
}

void	Command::UnlockChannelListMutex(void) {
	this->server_->UnlockChannelListMutex();
}

Channel	*Command::DeleteChannel(const std::string& channel_name) {
	return this->server_->DeleteChannel(channel_name);
}

void	Command::CeaseChannel(const std::string& channel_name) {
	this->server_->CeaseChannel(channel_name);
}

bool	Command::SearchChannelByName(const std::string& name) {
	return this->server_->SearchChannelByName(name);
}

std::string		Command::SearchClientBySock(const int& sock) {
	return this->server_->SearchClientBySock(sock);
}

int		Command::SearchClientByNick(const std::string& nick) {
	return this->server_->SearchClientByNick(nick);
}

void	Command::AddDeleteClient(const int& sock) {
	this->server_->AddDeleteClient(sock);
}

void	Command::CreateChannel(const channel_info& info) {
	this->server_->CreateChannel(info);
}

bool	Command::AddChannel(Channel *channel) {
	return this->server_->AddChannel(channel);
}

bool	Command::AddClient(Client *client) {
	return this->server_->AddClient(client);
}

Client	*Command::DeleteClient(const int& sock) {
	return this->server_->DeleteClient(sock);
}

std::map<std::string, Channel*>& Command::get_channels(void) {
	return this->server_->get_channels();
}

Channel	*Command::get_channel_ptr(const std::string& name) {
	return this->server_->get_channel_ptr(name);
}
