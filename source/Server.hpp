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
#include "Mutex.hpp"
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

		/* for command process */
		std::string		SearchClientBySock(const int& sock);
		int		SearchClientByNick(const std::string& nick);
		bool	SearchChannelByName(const std::string& name);
		void	AddDeleteClient(const int& sock);
	
	/* queries for command process */
		std::map<std::string, Channel>&	get_channels(void);
		bool	get_channel_members(std::map<int, std::string>& ret, \
									const std::string& channel_name, \
									const int& flag);
		bool	AddChannelMember(const std::string& channel_name, \
								const int& flag, \
								const int& sock);

	/* mutex list functions */
		bool	AddClientMutex(const int& sock);
		bool	AddChannelMutex(const std::string& name);
		bool	DeleteClientMutex(const int& sock);
		bool	DeleteChannelMutex(const std::string& name);

		bool	LockClientMutex(const int& sock);
		bool	LockChannelMutex(const std::string& name);
		void	UnlockClientMutex(const int& sock);
		void	UnlockChannelMutex(const std::string& name);

		bool	LockClientListMutex(void);
		void	UnlockClientListMutex(void);
		bool	LockChannelListMutex(void);
		void	UnlockChannelListMutex(void);

	/* Authentication */
		bool	AuthPassword(const std::string& password);

	/* InviteCommand */
		int		CheckInviteError(const std::string& chan, int receiver, int sender);
		int		CheckModeError(const std::string& chan, int mode);
		void	RunModeInServer(const std::vector<std::string>& params, char* mode_list);
	
	/* private member variables */

	protected:
		std::string	name_;
		
		int	sock_;
		int	port_;
		struct sockaddr_in	addr_;
		std::string	password_;
		struct timespec	timeout_;
	
		int	kq_;
		struct kevent	evlist_[FT_KQ_EVENT_SIZE];
		std::vector<struct kevent>	chlist_;

		std::map<int, std::string>		buffers_;
		std::map<int, Client>			clients_;//일단, socket fd 를 key로 지정
		Mutex							clients_mutex_;
		
		std::map<std::string, Channel>	channels_;
		Mutex							channels_mutex_;
		
		std::set<int>					del_clients_;
		Mutex							del_clients_mutex_;

		Mutex							list_mutex_;
		std::map<int, Mutex*>			client_mutex_list_;
		std::map<std::string, Mutex*>	channel_mutex_list_;

	/* private member functions*/
	private:
		void	MutexInit(void);
		void	ServerSocketInit(void);
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

	/* wooseoki functions */
		void	print_event(struct kevent *event, int i);
	
	friend class Command;
	friend class JoinCommand;
	friend class InviteCommand;
	friend class TopicCommand;
};

#endif
