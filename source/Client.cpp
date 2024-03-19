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
	if (this->auth_flag_ & AUTH)
		return true;
	return false;
}

void	Client::SetAuthFlag(const int& flag) {
	if (flag & AUTH_PASS)
		this->auth_flag_ |= AUTH_PASS;
	if (flag & AUTH_NICK)
		this->auth_flag_ |= AUTH_NICK;
	if (flag & AUTH_USER)
		this->auth_flag_ |= AUTH_USER;
	if (flag & AUTH)
		this->auth_flag_ |= AUTH;
}

char	Client::get_auth_flag(const int& flag) const {
	char	ret = 0;
	if ((flag & AUTH) && (this->auth_flag_ & AUTH))
		ret |= AUTH;
	if ((flag & AUTH_PASS) && (this->auth_flag_ & AUTH_PASS))
		ret |= AUTH_PASS;
	if ((flag & AUTH_NICK) && (this->auth_flag_ & AUTH_NICK))
		ret |= AUTH_NICK;
	if ((flag & AUTH_USER) && (this->auth_flag_ & AUTH_USER))
		ret |= AUTH_USER;
	return ret;
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
