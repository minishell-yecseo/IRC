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
#define FT_AUTH_PASS	(1 << 2)
#define FT_AUTH_NICK	(1 << 1)
#define FT_AUTH_USER	(1)

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
		void	SetAuthFlag(const int& flag);
		Client operator = (const Client& client);
		bool operator < (const Client& client) const;
		bool operator > (const Client& client) const;
		bool operator == (const Client& client) const;
		std::vector<std::string>	channels_;

	private:
		//int	id;//UNIQUE
		char	auth_flag_;
		int		sock_;
		std::string	nick_;//UNIQUE
		std::string	user_;
		struct sockaddr_in	addr_;
		socklen_t	addr_size_;
		std::string	password_;//sent from client at first access
};

#endif
