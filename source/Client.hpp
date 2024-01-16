#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <iostream>

class Server;
class Client {
	friend class Server;
	private:
		//int	id;//UNIQUE
		bool auth_;
		int	sock_;
		std::string	nick_;//UNIQUE
		std::string	user_;
		struct sockaddr_in	addr_;
		socklen_t	addr_size_;
		std::string	password_;//sent from client at first access
		std::string	buffer_;
	
	public:
		Client(void);
		int set_sock(int fd);
		bool operator < (const Client& client) const;
		bool operator > (const Client& client) const;
		bool operator == (const Client& client) const;
};

#endif
