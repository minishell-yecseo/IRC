#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>

class Server;
class Client {
	friend class Server;
	private:
		//int	id;//UNIQUE
		int	sock;
		std::string	nick;//UNIQUE
		std::string	user;
		struct sockaddr_in	addr;
		socklen_t	addr_size;
		std::string	password;//sent from client at first access
		std::string	buffer;
	
	public:
		bool operator < (const Client& client) const;
		bool operator > (const Client& client) const;
		bool operator == (const Client& client) const;
};

#endif
