#include "Client.hpp"

Client::Client(void) {
	sock_ = FT_INIT_CLIENT_FD;
	memset(&addr_, 0, sizeof(addr_));
	nick_ = "unknown_nick";
	user_ = "unknown_user";
	auth_flag_ = 0;
}

bool	Client::IsAuth(void) {
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

char	Client::get_auth_flag(const int& flag) {
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

	auth_flag_ = client.auth_flag_;
	sock_ = client.sock_;
	nick_ = client.nick_;
	user_ = client.user_;
	addr_ = client.addr_;
	addr_size_ = client.addr_size_;
	password_ = client.password_;
	return *this;
}

int	Client::set_sock(int fd) {
	if (fd > 0)
		sock_ = fd;
	return sock_;
}

const int& Client::get_sock(void) {
	return sock_;
}

void Client::set_nick(const std::string& nick) {
	this->nick_ = nick;
}

const std::string& Client::get_nick(void) {
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

const std::vector<std::string>&	Client::get_channels(void) {
	return this->channels_;
}

void	Client::add_channel(const std::string& channel_name) {
	this->channels_.push_back(channel_name);
}
