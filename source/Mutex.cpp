#include "Mutex.hpp"

Mutex::Mutex(void) {
}

Mutex::~Mutex(void) {
	destroy();
}

int	Mutex::init(const pthread_mutexattr_t *attr) {
	return pthread_mutex_init(&this->mutex_, attr);
}

int	Mutex::destroy(void) {
	return pthread_mutex_destroy(&this->mutex_);
}

int	Mutex::lock(void) throw(Mutex::LockError) {
	int ret = pthread_mutex_lock(&this->mutex_);
	if (ret != 0)
		throw (LockError());
	return ret;
}

int	Mutex::unlock(void) {
	return pthread_mutex_unlock(&this->mutex_);
}

const char * Mutex::LockError::what(void) const throw() {
	return "Mutex Lock Error";
}
