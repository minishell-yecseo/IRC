#include "ThreadPool.hpp"

ThreadPool::ThreadPool()
{
}

ThreadPool::ThreadPool(int size)
{
	this->thread_count = size;
	pthread_mutex_init(&(this->lock), NULL);
	pthread_cond_init(&(this->notify), NULL);

	this->threads.resize(size);
	for (int i = 0; i < this->thread_count; ++i)
	{
		pthread_create(&(this->threads[i]), NULL,
					ThreadPool::Worker, (void *)this);
	}
}

ThreadPool::~ThreadPool()
{
	pthread_mutex_lock(&(this->lock));
	this->shutdown = true;
	pthread_cond_broadcast(&(this->notify));
	pthread_mutex_unlock(&(this->lock));

	for (int i = 0; i < this->thread_count; ++i)
	{
		pthread_join(this->threads[i], NULL);
	}
	pthread_mutex_destroy(&(this->lock));
	pthread_cond_destroy(&(this->notify));
}

void	ThreadPool::Enqueue(void *arg)
{
	Command	*c;

	c = (Command *)arg;

	pthread_mutex_lock(&(this->lock));
	if (this->shutdown == true)
		return ;
	std::cout << "Enque\n";
	this->queue.push(c);
	this->count += 1;
	pthread_cond_signal(&(this->notify));
	pthread_mutex_unlock(&(this->lock));
}

void	*ThreadPool::Worker(void *arg)
{
	ThreadPool	*pool = (ThreadPool *)arg;
	Command	*c;

	for (;;)
	{
		pthread_mutex_lock(&(pool->lock));

		while ((pool->count == 0) && (pool->shutdown == false))
		{
			pthread_cond_wait(&(pool->notify), &(pool->lock));
		}
		if (pool->shutdown == true)
		{
			pthread_mutex_unlock(&(pool->lock));
			break ;
		}

		std::cout << "Run\n";
		c = pool->queue.front();
		pool->queue.pop();
		pool->count -= 1;

		pthread_mutex_unlock(&(pool->lock));

		c->Run();
	}


	pthread_mutex_unlock(&(pool->lock));
	pthread_exit(NULL);
	return NULL;
}
