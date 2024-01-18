#include "log.hpp"

using namespace log;

const char * log::endl = "\n";
log::ostream	log::cout;

extern pthread_mutex_t print;

log::ostream& log::ostream::operator << (char *char_ptr) {
	pthread_mutex_lock(&print);
	printf("%s", char_ptr);
	pthread_mutex_unlock(&print);
	return *this;
}

log::ostream& log::ostream::operator << (const char *char_ptr) {
	pthread_mutex_lock(&print);
	printf("%s", char_ptr);
	pthread_mutex_unlock(&print);
	return *this;
}

log::ostream& log::ostream::operator << (int i) {
	pthread_mutex_lock(&print);
	printf("%d", i);
	pthread_mutex_unlock(&print);
	return *this;
}

log::ostream& log::ostream::operator << (std::string str) {
	pthread_mutex_lock(&print);
	printf("%s", str.c_str());
	pthread_mutex_unlock(&print);
	return *this;
}
