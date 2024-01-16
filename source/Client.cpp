#include "Client.hpp"

Client::Client(void)
{
	auth_ = false;
	sock = -1;
	memset(&addr, 0, sizeof(addr));
}

int	Client::set_sock(int fd)
{
	if (fd > 0)
		sock = fd;
	return sock;
}

bool Client::operator < (const Client& client) const {
	return (this->sock < client.sock);
}

bool Client::operator > (const Client& client) const {
	return (this->sock > client.sock);
}

bool Client::operator == (const Client& client) const {
	if ((this->nick).compare(client.nick) == 0)
		return true;
	return false;
}
