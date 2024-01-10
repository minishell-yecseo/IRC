#ifndef SERVER_HPP
# define SERVER_HPP

#include <sys/socket.h>
#include <iostream>
#include <map>
#include <string>
#include "Utils.hpp"

#define FT_SOCK_QUEUE_SIZE 100
#define FT_KQ_EVENT_SIZE 100
#define FT_TIMEOUT_SEC 5
#define FT_TIMEOUT_NSEC 0
#define FT_BUFF_SIZE 1024

class Client;
class Channel;
class Server {
	private:
		int							socket;
		struct sockaddr_in			addr;
		int							port;
		std::string					password;
		struct timespec				timeout;
	
		int							kq;
		struct kevent				evlist[FT_KQ_EVENT_SIZE];
		vector<struct kevent>		chlist;

		map<int, Client>			clients;//일단, socket fd 를 key로 지정
		map<std::string, Channel>	channels;

	public:
		Server(int argc, char **argv);
		bool	run(void);
	
	private:
		void	server_socket_init(void);
		void	kqueue_init(void);
		void	handle_events(int nev);
		void	handle_timeout(void);
		void	handle_event_error(struct kevent event);
		void	handle_client_event(struct kevent event);
		void	disconnect_client(struct kevent event);
		void	connect_client(void);
		void	add_event(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
	
	/* wooseoki functions */
	private:
		void	print_event(struct kevent *event, int i);
};

#endif
