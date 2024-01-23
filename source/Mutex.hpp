#ifndef MUTEX_HPP
# define MUTEX_HPP

#include <pthread.h>
#include <exception>
#include <string>

class	Mutex {
	public:
		Mutex(void);
		~Mutex(void);
		int	lock(void) throw(LockError);
		int	unlock(void);
		int	init(const pthread_mutexattr_t *attr = NULL);
	
		class LockError : public std::exception {
			virtual const char * what(void) const throw();
		};

	private:
		pthread_mutex_t	mutex_;
		int	destroy(void);

};

#endif
