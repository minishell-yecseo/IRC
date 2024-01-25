#include "log.hpp"

using namespace log;

const char * log::endl = "\n";
log::ostream	log::cout;

extern Mutex print;

log::ostream& log::ostream::operator << (char *char_ptr) {
	print.lock();
	printf("%s", char_ptr);
	print.unlock();
	return *this;
}

log::ostream& log::ostream::operator << (const char *char_ptr) {
	print.lock();
	printf("%s", char_ptr);
	print.unlock();
	return *this;
}

log::ostream& log::ostream::operator << (int i) {
	print.lock();
	printf("%d", i);
	print.unlock();
	return *this;
}

log::ostream& log::ostream::operator << (std::string str) {
	print.lock();
	printf("%s", str.c_str());
	print.unlock();
	return *this;
}

log::ostream& log::ostream::operator << (pthread_t tid) {
	print.lock();
	printf("%lu", (unsigned long)tid);
	print.unlock();
	return *this;
}
