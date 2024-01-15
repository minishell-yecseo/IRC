#include "Client.hpp"

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
