#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <iostream>
#include <vector>

#define FT_INIT_CLIENT_FD -1
#define FT_INIT_AUTH -3

class Server;

class Client {
	friend class Server;
	public:
		Client(void);
		int set_sock(int fd);
		void set_nick(const std::string& nick);
		const int& get_sock(void);
		const std::string& get_nick(void);
		bool	IsAuth(void);
		void	AuthIncrease(void);
		Client operator = (const Client& client);
		bool operator < (const Client& client) const;
		bool operator > (const Client& client) const;
		bool operator == (const Client& client) const;
		std::vector<std::string>	channels_;

	private:
		//int	id;//UNIQUE
		int8_t	auth_;
		int		sock_;
		std::string	nick_;//UNIQUE
		std::string	user_;
		struct sockaddr_in	addr_;
		socklen_t	addr_size_;
		std::string	password_;//sent from client at first access
};

#endif
