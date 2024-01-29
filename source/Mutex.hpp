#ifndef MUTEX_HPP
# define MUTEX_HPP

#include <exception>
#include <pthread.h>
#include <string>

class	Mutex {
	public:
		Mutex(void);
		~Mutex(void);
		int	lock(void);
		int	unlock(void);
		int	init(const pthread_mutexattr_t *attr = NULL);
	
		class FailLock : public std::exception
		{
			virtual const char* what() const throw();
		};

	private:
		pthread_mutex_t	mutex_;
		int	destroy(void);
};

#endif
