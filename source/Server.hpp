#ifndef SERVER_HPP
# define SERVER_HPP

#include <sys/socket.h>
#include <sys/event.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <string>

class Command;
class Channel;
class Client;
class Server;

#include "Request.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "ThreadPool.hpp"
#include "Utils.hpp"
#include "log.hpp"

#define FT_SOCK_QUEUE_SIZE 100
#define FT_KQ_EVENT_SIZE 100
#define FT_TIMEOUT_SEC 5
#define FT_TIMEOUT_NSEC 0
#define FT_BUFF_SIZE 1024
#define FT_THREAD_POOL_SIZE 10
# ifndef FT_SERVER_NAME
#  define FT_SERVER_NAME "Happy"
# endif

class Server {
	public:
		~Server();
		Server(int argc, char **argv);
		bool	Run(void);
		ThreadPool	*pool_;

		const std::string& get_name(void);
		const int&	get_port(void);
		const struct sockaddr_in&	get_addr(void);
		pthread_mutex_t *get_s_clients_mutex(void);
		pthread_mutex_t *get_s_channels_mutex(void);

		/* for command process */
		int	SearchClientByNick(const std::string& nick);
		//int	SearchChannelByName(const std::string& name);

	/* private member variables */
	private:
		std::string	name_;

		int	sock_;
		int	port_;
		struct sockaddr_in	addr_;
		std::string	password_;
		struct timespec	timeout_;
	
		int	kq_;
		struct kevent	evlist_[FT_KQ_EVENT_SIZE];
		std::vector<struct kevent>	chlist_;

		std::map<int, Client>	clients_;//일단, socket fd 를 key로 지정
		std::map<int, std::string>	buffers_;
		std::map<std::string, Channel>	channels_;

	/* private member functions*/
	private:
		void	ServerSocketInit(void);
		void	KqueueInit(void);
		void	HandleEvents(int nev);
		void	HandleTimeout(void);
		void	HandleEventError(struct kevent event);
		void	HandleClientEvent(struct kevent event);
		void	DisconnectClient(struct kevent event);
		void	ConnectClient(void);
		void	AddEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);

	/* Authentication */
		bool	AuthClient(Client& client);
	
	/* debugging functions */
		void	p_event_filter(struct kevent *event);
		void	p_event_flags(struct kevent*event);

	/* wooseoki functions */
		void	print_event(struct kevent *event, int i);
};

#endif
