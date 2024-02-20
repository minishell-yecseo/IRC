#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int size) {
	this->count_ = 0;
	this->thread_count_ = size;
	pthread_mutex_init(&(this->lock_), NULL);
	pthread_cond_init(&(this->notify_), NULL);
	this->threads_.resize(size);
	for (int i = 0; i < this->thread_count_; ++i) {
		pthread_create(&(this->threads_[i]), NULL,
						ThreadPool::Worker,
						(void *)this);
	}
}

ThreadPool::~ThreadPool() {
	pthread_mutex_lock(&(this->lock_));
	this->shutdown_ = true;
	pthread_cond_broadcast(&(this->notify_));
	pthread_mutex_unlock(&(this->lock_));

	for (int i = 0; i < this->thread_count_; ++i) {
		pthread_join(this->threads_[i], NULL);
	}
	pthread_mutex_destroy(&(this->lock_));
	pthread_cond_destroy(&(this->notify_));
}

void	ThreadPool::Enqueue(void *arg) {
	Command	*c;

	c = (Command *)arg;

	pthread_mutex_lock(&(this->lock_));
	if (this->shutdown_ == true)
		return ;
	this->queue_.push(c);
	this->count_ += 1;
	pthread_cond_signal(&(this->notify_));
	pthread_mutex_unlock(&(this->lock_));
}

void	ThreadPool::UnblockSignal(void) {
	struct sigaction	act;

	act.sa_handler = HandleSIGPIPE;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGPIPE, &act, 0);
}

void	*ThreadPool::Worker(void *arg) {
	ThreadPool	*pool = (ThreadPool *)arg;
	Command	*c = NULL;

	pool->UnblockSignal();
	for (;;) {
		pthread_mutex_lock(&(pool->lock_));

		while ((pool->count_ == 0) && (pool->shutdown_ == false)) {
			pthread_cond_wait(&(pool->notify_), &(pool->lock_));
		}
		if (pool->shutdown_ == true) {
			pthread_mutex_unlock(&(pool->lock_));
			break ;
		}
		if (pool->queue_.empty() == false) {
			c = pool->queue_.front();
			pool->queue_.pop();
			pool->count_ -= 1;
		}
		pthread_mutex_unlock(&(pool->lock_));
		if (c == NULL)
			continue ;
		c->Run();
		delete c;
		c = NULL;
	}

	pthread_mutex_unlock(&(pool->lock_));
	pthread_exit(NULL);
	return NULL;
}
