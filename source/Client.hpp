#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <iostream>
#include <vector>

class Server;

#define FT_INIT_CLIENT_FD -1
#define FT_AUTH				(1 << 3)
#define FT_AUTH_PASS		(1 << 2)
#define FT_AUTH_NICK		(1 << 1)
#define FT_AUTH_USER		(1)
#define	FT_AUTH_ALL			(FT_AUTH_PASS | FT_AUTH_NICK | FT_AUTH_USER)

typedef struct _ClientNetInfo {
	int 				sock;
	struct sockaddr_in	addr;
	socklen_t			addr_size;
} ClientNetInfo;

class Client {
	public:
		Client(int sock);
		void	Init(int sock = FT_INIT_CLIENT_FD);

		bool	IsAuth(void) const;
		void	SetAuthFlag(const int& flag);
		void	add_channel(const std::string& channel_name);

		//getters
		const std::vector<std::string>&	get_channels(void) const;
		const std::string&				get_host_name(void) const;
		const std::string&				get_server_name(void) const;
		const std::string&				get_user_name(void) const;
		const int&						get_sock(void) const;
		const std::string&				get_nick(void) const;
		char							get_auth_flag(const int& flag = FT_AUTH_ALL) const;

		//setters
		int		set_sock(int fd);
		void	set_nick(const std::string& nick);
		void	set_user_name(const std::string& user_name);
		void 	set_host_name(const std::string& host_name);
		void 	set_server_name(const std::string& server_name);
		void 	set_real_name(const std::string& real_name);

		//test
		void	UnsetAuthFlagInTest(void);

	private:
		int		sock_;
		char	auth_flag_;
		std::string	nick_;
		std::string	user_name_;
		std::string	host_name_;
		std::string	server_name_;
		std::string	real_name_;
		std::string	password_;
		std::vector<std::string>	channels_;
		ClientNetInfo		address_;

		//operators
		Client operator = (const Client& client);
		bool operator < (const Client& client) const;
		bool operator > (const Client& client) const;
		bool operator == (const Client& client) const;

		friend class Server;
};
#endif
