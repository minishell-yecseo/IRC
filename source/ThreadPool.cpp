#include "ThreadPool.hpp"

ThreadPool::ThreadPool() {
}

ThreadPool::ThreadPool(int size) {
	this->thread_count_ = size;
	pthread_mutex_init(&(this->lock_), NULL);
	pthread_mutex_init(&(this->list_mutex_), NULL);
	pthread_mutex_init(&(this->server_clients_mutex_), NULL);
	pthread_mutex_init(&(this->server_channels_mutex_), NULL);
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
	//std::cout << "Enque\n";
	this->queue_.push(c);
	this->count_ += 1;
	pthread_cond_signal(&(this->notify_));
	pthread_mutex_unlock(&(this->lock_));
}

void	*ThreadPool::Worker(void *arg) {
	ThreadPool	*pool = (ThreadPool *)arg;
	Command	*c;

	for (;;) {
		pthread_mutex_lock(&(pool->lock_));

		while ((pool->count_ == 0) && (pool->shutdown_ == false)) {
			pthread_cond_wait(&(pool->notify_), &(pool->lock_));
		}
		if (pool->shutdown_ == true) {
			pthread_mutex_unlock(&(pool->lock_));
			break ;
		}

		//std::cout << "Run\n";
		c = pool->queue_.front();
		pool->queue_.pop();
		pool->count_ -= 1;

		pthread_mutex_unlock(&(pool->lock_));

		c->Run();
		delete c;
	}

	pthread_mutex_unlock(&(pool->lock_));
	pthread_exit(NULL);
	return NULL;
}

bool	ThreadPool::AddClientMutex(const int& sock) {
	std::cout << BOLDRED << "AddClientMutex call with socket: " << sock << "\n" << RESET;
	int	ret;
	pthread_mutex_t *mutex_ptr = new pthread_mutex_t();
	pthread_mutex_lock(&list_mutex_);//lock
	std::map<int, pthread_mutex_t*>::iterator	mutex_it = client_mutex_list_.find(sock);
	if (mutex_it != client_mutex_list_.end()) {
		pthread_mutex_unlock(&list_mutex_);//unlock
		std::cout << MAGENTA << "socket(" << sock << ") has mutex already\n" << RESET;
		return false;
	}
	client_mutex_list_.insert(std::make_pair(sock, mutex_ptr));
	ret = ft_mutex_init(mutex_ptr);
	if (!ret)
		client_mutex_list_.erase(sock);
	pthread_mutex_unlock(&list_mutex_);//unlock
	std::cout << BOLDRED << "AddClientMutex ret: " << ret << "\n" << RESET;
	return ret;
}

bool	ThreadPool::AddChannelMutex(const std::string& name) {
	int	ret;
	pthread_mutex_t *mutex_ptr = new pthread_mutex_t();
	pthread_mutex_lock(&list_mutex_);//lock
	std::map<std::string, pthread_mutex_t*>::iterator	mutex_it = channel_mutex_list_.find(name);
	if (mutex_it != channel_mutex_list_.end())
		return false;
	channel_mutex_list_.insert(std::make_pair(name, mutex_ptr));
	ret = ft_mutex_init(mutex_ptr);
	if (!ret)
		channel_mutex_list_.erase(name);
	pthread_mutex_unlock(&list_mutex_);//unlock
	return ret;
}

bool	ThreadPool::DeleteClientMutex(const int& sock) {
	std::cout << BLUE << "DeleteClientMutex call with socket: " << sock << RESET << "\n";
	int	ret;
	pthread_mutex_lock(&list_mutex_);//lock
	std::map<int, pthread_mutex_t*>::iterator	mutex_it = client_mutex_list_.find(sock);
	if (mutex_it == client_mutex_list_.end()) {
		std::cerr << BLUE << "There's no mutex about sock(" << sock << ")\n" << RESET;
		pthread_mutex_unlock(&list_mutex_);//unlock
		return false;
	}
	ret = ft_mutex_destroy(mutex_it->second);//ERROR SECTION
	delete mutex_it->second;
	client_mutex_list_.erase(mutex_it);
	pthread_mutex_unlock(&list_mutex_);//unlock
	return ret;
}

bool	ThreadPool::DeleteChannelMutex(const std::string& name) {
	int	ret;
	pthread_mutex_lock(&list_mutex_);//lock
	std::map<std::string, pthread_mutex_t*>::iterator	mutex_it = channel_mutex_list_.find(name);
	if (mutex_it != channel_mutex_list_.end())
		return false;
	ret = ft_mutex_destroy(mutex_it->second);
	delete mutex_it->second;
	channel_mutex_list_.erase(mutex_it);
	pthread_mutex_unlock(&list_mutex_);//unlock
	return ret;
}

bool	ThreadPool::LockClientMutex(const int& sock) {
	std::cout << "LockClientMutex call! sock: " << sock << "\n";
	int	ret;
	pthread_mutex_lock(&list_mutex_);//lock
	std::map<int, pthread_mutex_t*>::iterator	mutex_it = client_mutex_list_.find(sock);
	if (mutex_it == client_mutex_list_.end()) {
		std::cerr << "LockClientMutexLock() fail\n";
		pthread_mutex_unlock(&list_mutex_);//unlock
		return false;
	}
	pthread_mutex_unlock(&list_mutex_);//unlock
	ret = ft_mutex_lock(mutex_it->second);
	return ret;
}

bool	ThreadPool::LockChannelMutex(const std::string& name) {
	std::cout << "LockChannelMutex call! name: " << name << "\n";
	int	ret;
	pthread_mutex_lock(&list_mutex_);//lock
	std::map<std::string, pthread_mutex_t*>::iterator	mutex_it = channel_mutex_list_.find(name);
	if (mutex_it == channel_mutex_list_.end()) {
		std::cerr << "LockChannelMutexLock() fail\n";
		pthread_mutex_unlock(&list_mutex_);//unlock
		return false;
	}
	pthread_mutex_unlock(&list_mutex_);//unlock
	ret = ft_mutex_lock(mutex_it->second);
	return ret;
}

void	ThreadPool::UnlockClientMutex(const int& sock) {
	std::cout << "UnlockClientMutex call! sock: " << sock << "\n";
	pthread_mutex_lock(&list_mutex_);//lock
	std::map<int, pthread_mutex_t*>::iterator	mutex_it = client_mutex_list_.find(sock);
	if (mutex_it == client_mutex_list_.end()) {
		std::cerr << "UnlockClientMutexUnLock() fail\n";
		pthread_mutex_unlock(&list_mutex_);//unlock
		return ;
	}
	ft_mutex_unlock(mutex_it->second);
	pthread_mutex_unlock(&list_mutex_);//unlock
}

void	ThreadPool::UnlockChannelMutex(const std::string& name) {
	std::cout << "UnlockChannelMutex call! name: " << name << "\n";
	pthread_mutex_lock(&list_mutex_);//lock
	std::map<std::string, pthread_mutex_t*>::iterator	mutex_it = channel_mutex_list_.find(name);
	if (mutex_it == channel_mutex_list_.end()) {
		std::cerr << "UnlockChannelMutexUnLock() fail\n";
		pthread_mutex_unlock(&list_mutex_);//unlock
		return ;
	}
	ft_mutex_unlock(mutex_it->second);
	pthread_mutex_unlock(&list_mutex_);//unlock
}

bool	ThreadPool::ft_mutex_init(pthread_mutex_t *mutex) {
	int ret = pthread_mutex_init(mutex, NULL);
	if (ret != 0) {
		std::cerr << strerror(ret) << "\n";
		return false;
	}
	return true;
}

bool	ThreadPool::ft_mutex_destroy(pthread_mutex_t *mutex) {
	int	ret = pthread_mutex_destroy(mutex);
	if (ret != 0)
		return false;
	return true;
}

bool	ThreadPool::ft_mutex_lock(pthread_mutex_t *mutex) {
	int ret = pthread_mutex_lock(mutex);
	if (ret != 0) {
		std::cerr << "error:" << strerror(ret) << "\n";
		return false;
	}
	return true;
}

void	ThreadPool::ft_mutex_unlock(pthread_mutex_t *mutex) {
	int ret = pthread_mutex_unlock(mutex);
	if (ret != 0) {
		std::cerr << strerror(ret) << "\n";
		return;
	}
	return;
}
