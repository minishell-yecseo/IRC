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

int	Mutex::lock(void) {
	int ret = pthread_mutex_lock(&this->mutex_);
	if (ret != 0)
		throw FailLock();
	return ret;
}

int	Mutex::unlock(void) {
	return pthread_mutex_unlock(&this->mutex_);
}

const char*	Mutex::FailLock::what(void) const throw() {
	return "Mutex : Lock Failed";
}
