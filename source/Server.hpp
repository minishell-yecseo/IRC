#ifndef SERVER_HPP
# define SERVER_HPP

#include <sys/socket.h>
#include <sys/event.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <exception>

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
#include "ServerInterface.hpp"

#define SOCK_QUEUE_SIZE 100
#define KQ_EVENT_SIZE 100
#define TIMEOUT_SEC 5
#define TIMEOUT_NSEC 0
#define BUFF_SIZE 1024
#define THREAD_POOL_SIZE 10
# ifndef SERVER_NAME
#  define SERVER_NAME "Happy"
# endif
# ifndef IRC_VERSION
#  define IRC_VERSION "0.1"
# endif

class Server: public ServerInterface {
	public:
		~Server();
		Server(int argc, char **argv);
		void		Run(void);

	/* private member functions*/
	private:
		const std::string&	get_name(void);
		const std::string&	get_version(void);
		const std::string&	get_create_time(void);
		std::string			set_create_time(void);

		void	MutexInit(void);
		void	ServerSocketInit(char **argv);
		void	KqueueInit(void);
		void	SignalInit(void);
		void	HandleEvents(int nev);
		void	HandleEventError(struct kevent event);
		void	HandleClientEvent(struct kevent event);
		void	AreValidArgs(int argc, char**argv);
		bool	IsValidPort(char *argv);
		void	DeleteInvalidClient(void);
		void	DisconnectClient(const int& sock);//free Client
		void	DeleteClientInChannel(const int& sock, Client *client);
		void	DeleteClientEvent(const int& sock);
		void	ReadSocket(Client *client);

		/* mutex list functions */
		bool	AddClientMutex(const int& sock);
		bool	DeleteClientMutex(const int& sock);
		bool	LockClientMutex(const int& sock);
		void	UnlockClientMutex(const int& sock);
		bool	LockClientListMutex(void);
		void	UnlockClientListMutex(void);

		bool	AddChannelMutex(const std::string& name);
		bool	DeleteChannelMutex(const std::string& name);
		bool	LockChannelMutex(const std::string& name);
		void	UnlockChannelMutex(const std::string& name);
		bool	LockChannelListMutex(void);
		void	UnlockChannelListMutex(void);

		/* for command process */
		std::string		SearchClientBySock(const int& sock);
		int		SearchClientByNick(const std::string& nick);
		bool	SearchChannelByName(const std::string& name);
		void	AddDeleteClient(const int& sock);
		void	CreateChannel(const channel_info& info);//allocate Channel
		bool	AddChannel(Channel *channel);
		bool	AddClient(Client *client);
		
		/* Authentication */
		bool	AuthPassword(const std::string& password);

		void	CeaseChannel(const std::string& channel_name);//free Channel
		Channel	*DeleteChannel(const std::string& channel_name);
		Client	*DeleteClient(const int& sock);

		std::map<std::string, Channel*>& get_channels(void);
		Channel	*get_channel_ptr(const std::string& name);

		void			ConnectClient(void);//allocation Client
		ClientNetInfo	AcceptClient(void);
		void			AddEvent(uintptr_t ident, int16_t filter, uint16_t flags, \
						uint32_t fflags, intptr_t data, void *udata);
		void			p_server_info(void);

	template<typename k, typename v>
	void	delete_map(std::map<k, v> cont) {
		typedef typename std::map<k, v>::iterator	iterator;
		iterator	itr = cont.begin();
		while (itr != cont.end()) {
			delete itr->second;
			itr++;
		}
	}
		
	/* private member variables */
	private:
		int	kq_;
		int	sock_;
		int	port_;
		std::string	name_;
		std::string	version_;
		std::string	password_;
		std::string	createtime_;
		struct timespec		timeout_;
		struct sockaddr_in	addr_;
		struct sigaction	act;
		struct kevent	evlist_[KQ_EVENT_SIZE];
		
		ThreadPool	*pool_;
		
		std::map<int, std::string>		buffers_;

		std::map<int, Client*>			clients_;//일단, socket fd 를 key로 지정
		Mutex							clients_mutex_;
		
		std::vector<struct kevent>	chlist_;
		std::map<std::string, Channel*>	channels_;
		Mutex							channels_mutex_;
		
		std::set<int>					del_clients_;
		Mutex							del_clients_mutex_;

		Mutex							list_mutex_;
		std::map<int, Mutex*>			client_mutex_list_;
		std::map<std::string, Mutex*>	channel_mutex_list_;

		friend class	Command;
		friend class	TestCommand;
};

#endif
