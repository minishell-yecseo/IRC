#include "Client.hpp"

Client::Client(void)
{
	auth_ = false;
	sock_ = -1;
	memset(&addr_, 0, sizeof(addr_));
}

int	Client::set_sock(int fd)
{
	if (fd > 0)
		sock_ = fd;
	return sock_;
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
