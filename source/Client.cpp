#include "Client.hpp"

Client::Client(int sock) {
	Init(sock);
}

void	Client::Init(int sock) {
	this->sock_ = sock;
	memset(&(this->address_), 0, sizeof(ClientNetInfo));
	this->nick_ = "unknown";
	this->auth_flag_ = 0;
	this->user_name_ = "user";
	this->host_name_ = "host";
	this->server_name_ = "server";
	this->real_name_ = "real name";
}

bool	Client::IsAuth(void) const {
	if (this->auth_flag_ & FT_AUTH)
		return true;
	return false;
}

void	Client::SetAuthFlag(const int& flag) {
	if (flag & FT_AUTH_PASS)
		this->auth_flag_ |= FT_AUTH_PASS;
	if (flag & FT_AUTH_NICK)
		this->auth_flag_ |= FT_AUTH_NICK;
	if (flag & FT_AUTH_USER)
		this->auth_flag_ |= FT_AUTH_USER;
	if (flag & FT_AUTH)
		this->auth_flag_ |= FT_AUTH;
}

char	Client::get_auth_flag(const int& flag) const {
	char	ret = 0;
	if ((flag & FT_AUTH) && (this->auth_flag_ & FT_AUTH))
		ret |= FT_AUTH;
	if ((flag & FT_AUTH_PASS) && (this->auth_flag_ & FT_AUTH_PASS))
		ret |= FT_AUTH_PASS;
	if ((flag & FT_AUTH_NICK) && (this->auth_flag_ & FT_AUTH_NICK))
		ret |= FT_AUTH_NICK;
	if ((flag & FT_AUTH_USER) && (this->auth_flag_ & FT_AUTH_USER))
		ret |= FT_AUTH_USER;
	return ret;
}

Client Client::operator = (const Client& client) {
	if (this == &client)
		return *this;

	this->auth_flag_ = client.auth_flag_;
	this->sock_ = client.sock_;
	this->nick_ = client.nick_;
	this->user_name_ = client.user_name_;
	this->host_name_ = client.host_name_;
	this->server_name_ = client.server_name_;
	this->real_name_ = client.real_name_;
	this->address_ = client.address_;
	this->password_ = client.password_;
	return *this;
}

int	Client::set_sock(int fd) {
	if (fd > 0)
		sock_ = fd;
	return sock_;
}

const int& Client::get_sock(void) const {
	return sock_;
}

void Client::set_nick(const std::string& nick) {
	this->nick_ = nick;
}

const std::string& Client::get_nick(void) const {
	return this->nick_;
}

bool Client::operator < (const Client& client) const {
	return (this->sock_ < client.sock_);
}

bool Client::operator > (const Client& client) const {
	return (this->sock_ > client.sock_);
}

bool Client::operator == (const Client& client) const {
	if ((this->nick_).compare(client.nick_) == 0)
		return true;
	return false;
}

const std::vector<std::string>&	Client::get_channels(void) const {
	return this->channels_;
}

void	Client::add_channel(const std::string& channel_name) {
	this->channels_.push_back(channel_name);
}

void	Client::set_user_name(const std::string& user_name) {
	this->user_name_ = user_name;
}

void	Client::set_host_name(const std::string& host_name) {
	this->host_name_ = host_name;
}

void	Client::set_server_name(const std::string& server_name) {
	this->server_name_ = server_name;
}

void	Client::set_real_name(const std::string& real_name) {
	this->real_name_ = real_name;
}

const std::string&	Client::get_user_name(void) const {
	return this->user_name_;
}

const std::string&	Client::get_host_name(void) const {
	return this->host_name_;
}

const std::string&	Client::get_server_name(void) const {
	return this->server_name_;
}

void	Client::UnsetAuthFlagInTest(void) {
	this->auth_flag_ = 0;
}
