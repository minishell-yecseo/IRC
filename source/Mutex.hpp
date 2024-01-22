#ifndef MUTEX_HPP
# define MUTEX_HPP

#include <pthread.h>

class	Mutex {
	public:
		Mutex(void);
		~Mutex(void);
		int	lock(void);
		int	unlock(void);
		int	init(const pthread_mutexattr_t *attr = NULL);

	private:
		pthread_mutex_t	mutex_;
		int	destroy(void);
};

#endif
