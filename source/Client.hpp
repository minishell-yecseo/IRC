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
#define FT_AUTH				(1 << 3)
#define FT_AUTH_PASS		(1 << 2)
#define FT_AUTH_NICK		(1 << 1)
#define FT_AUTH_USER		(1)
#define	FT_AUTH_ALL			(FT_AUTH_PASS | FT_AUTH_NICK | FT_AUTH_USER)

class Server;

class Client {
	friend class Server;
	public:
		Client(void);
		int set_sock(int fd);
		void set_nick(const std::string& nick);
		void set_user_name(const std::string& user_name);
		void set_host_name(const std::string& host_name);
		void set_server_name(const std::string& server_name);
		void set_real_name(const std::string& real_name);
		const std::string&	get_host_name(void);
		const std::string&	get_server_name(void);
		const std::string&	get_user_name(void);
		const int& get_sock(void);
		const std::string& get_nick(void);
		bool	IsAuth(void);
		void	SetAuthFlag(const int& flag);
		char	get_auth_flag(const int& flag = FT_AUTH_ALL);
		Client operator = (const Client& client);
		bool operator < (const Client& client) const;
		bool operator > (const Client& client) const;
		bool operator == (const Client& client) const;
		void	add_channel(const std::string& channel_name);
		const std::vector<std::string>&	get_channels(void);

	private:
		//int	id;//UNIQUE
		char	auth_flag_;
		int		sock_;
		std::string	nick_;//UNIQUE
		std::string	user_name_;
		std::string	host_name_;
		std::string	server_name_;
		std::string	real_name_;

		struct sockaddr_in	addr_;
		socklen_t	addr_size_;
		std::string	password_;//sent from client at first access
		std::vector<std::string>	channels_;
};

#endif
