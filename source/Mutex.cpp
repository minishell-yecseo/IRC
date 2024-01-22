#include "Mutex.hpp"

Mutex::Mutex(void) {
}

Mutex::Mutex(const pthread_mutexattr_t *attr) {
	init(attr);
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
	return pthread_mutex_lock(&this->mutex_);
}

int	Mutex::unlock(void) {
	return pthread_mutex_unlock(&this->mutex_);
}
