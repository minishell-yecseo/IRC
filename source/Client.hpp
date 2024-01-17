#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <iostream>

#define FT_INIT_CLIENT_FD -1

class Server;
class Client {
	friend class Server;
	public:
		Client(void);
//		Client(const Client& client);
		int set_sock(int fd);
		const int& get_sock(void);
		Client operator = (const Client& client);
		bool operator < (const Client& client) const;
		bool operator > (const Client& client) const;
		bool operator == (const Client& client) const;

	private:
		//int	id;//UNIQUE
		bool auth_;
		int	sock_;
		std::string	nick_;//UNIQUE
		std::string	user_;
		struct sockaddr_in	addr_;
		socklen_t	addr_size_;
		std::string	channel_name_;
		std::string	password_;//sent from client at first access
};

#endif
