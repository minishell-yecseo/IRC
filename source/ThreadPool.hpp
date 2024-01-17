#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

class	Command;
class	Server;
class	Client;

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>
#include <queue>
#include <map>

#include "Command.hpp"

class ThreadPool
{
	public:
		std::vector<pthread_t>	threads_;
		std::queue<Command*> 	queue_;
		pthread_mutex_t			list_mutex_;
		pthread_mutex_t			s_clients_mutex_;
		pthread_mutex_t			s_channels_mutex_;
		pthread_mutex_t			lock_;
		pthread_cond_t			notify_;
		int						thread_count_;
		int						count_;
		bool					shutdown_;
		std::map<int, pthread_mutex_t*>	client_mutex_list_;
		std::map<std::string, pthread_mutex_t*>	channel_mutex_list_;

		ThreadPool(void);
		ThreadPool(int size);
		~ThreadPool();

		void		Enqueue(void *arg);
		static void	*Worker(void *arg);
		bool		AddClientMutex(const int& sock);
		bool		AddChannelMutex(const std::string& name);
		bool		DeleteClientMutex(const int& sock);
		bool		DeleteChannelMutex(const std::string& name);
		bool		LockClientMutex(const int& sock);
		bool		LockChannelMutex(const std::string& name);
		void		UnlockClientMutex(const int& sock);
		void		UnlockChannelMutex(const std::string& name);

	private:
		bool	ft_mutex_init(pthread_mutex_t *mutex);
		bool	ft_mutex_destroy(pthread_mutex_t *mutex);
		bool	ft_mutex_lock(pthread_mutex_t *mutex);
		void	ft_mutex_unlock(pthread_mutex_t *mutex);
};
#endif
