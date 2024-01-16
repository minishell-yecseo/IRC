#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <queue>
#include "Command.hpp"

class ThreadPool
{
	public:
		std::vector<pthread_t>	threads;
		std::queue<Command*> 	queue;
		pthread_mutex_t			lock;
		pthread_cond_t			notify;
		int						thread_count;
		int						count;
		bool					shutdown;

		ThreadPool(void);
		ThreadPool(int size);
		~ThreadPool();

		void		Enqueue(void *arg);
		static void	*Worker(void *arg);
};
#endif
