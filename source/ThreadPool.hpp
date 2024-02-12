#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

class	Command;
class	Server;
class	Client;

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <map>
#include <queue>
#include <map>

#include "Command.hpp"
#include "log.hpp"

class ThreadPool
{
	private:
		ThreadPool(int size);
		~ThreadPool();
		void		Enqueue(void *arg);
		static void	*Worker(void *arg);

		std::vector<pthread_t>	threads_;
		std::queue<Command*> 	queue_;
		pthread_mutex_t			lock_;
		pthread_cond_t			notify_;
		bool					shutdown_;
		int						thread_count_;
		int						count_;

		friend class	Server;

};
#endif
