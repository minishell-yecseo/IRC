#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <queue>

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
			
		ThreadPool(int size);
		~ThreadPool();

		void		Enqueue(void *arg);
		static void	*Worker(void *arg);
};
#endif
