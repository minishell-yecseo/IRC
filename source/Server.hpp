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
# ifndef IRC_VERSION
#  define IRC_VERSION "0.1"
# endif

class Server {
	public:
		~Server();
		Server(int argc, char **argv);
		bool	Run(void);

		const std::string& get_name(void);
		const std::string& get_version(void);
		const std::string& get_create_time(void);
		const int&	get_port(void);
		const struct sockaddr_in&	get_addr(void);
		std::map<std::string, Channel>& get_channels(void);
		Channel*	get_channel_ptr(const std::string& name);
		std::string	set_create_time(void);

		/* for command process */
		std::string		SearchClientBySock(const int& sock);
		int		SearchClientByNick(const std::string& nick);
		bool	SearchChannelByName(const std::string& name);
		void	AddDeleteClient(const int& sock);

		void	CeaseChannel(const std::string& channel_name);
		void	DeleteChannel(const std::string& channel_name);
	
	/* queries for command process */
		bool	get_channel_members(std::map<int, std::string>* ret, \
									const std::string& channel_name, \
									const int& flag);
		bool	AddChannelMember(const std::string& channel_name, \
								const int& flag, \
								const int& sock);

	/* Authentication */
		bool	AuthPassword(const std::string& password);

	/* private member variables */
	protected:
		std::string	name_;
		
		int	sock_;
		int	port_;
		struct sockaddr_in	addr_;
		std::string	password_;
		struct timespec	timeout_;
		std::string	version_;
		std::string	createtime_;
	
		int	kq_;
		struct kevent	evlist_[FT_KQ_EVENT_SIZE];
		std::vector<struct kevent>	chlist_;

		std::map<int, std::string>		buffers_;
		std::map<int, Client>			clients_;
		std::map<std::string, Channel>	channels_;
		std::set<int>					del_clients_;

	/* private member functions*/
	private:
		void	ServerSocketInit(char **argv);
		void	KqueueInit(void);
		void	HandleEvents(int nev);
		void	HandleTimeout(void);
		void	HandleEventError(struct kevent event);
		void	HandleClientEvent(struct kevent event);
		void	DeleteInvalidClient(void);
		void	DisconnectClient(const int& sock);
		void	ConnectClient(void);
		void	AddEvent(uintptr_t ident, int16_t filter, uint16_t flags, \
						uint32_t fflags, intptr_t data, void *udata);

	/* debugging functions */
		void	p_event_filter(struct kevent *event);
		void	p_event_flags(struct kevent*event);
		void	p_server_info(void);
		void	print_channels(void);
		void	print_clients(void);
	
	/* wooseoki functions */
		void	print_event(struct kevent *event, int i);
	
	friend class Command;
	friend class JoinCommand;
};

#endif
