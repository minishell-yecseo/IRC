#include "log.hpp"

using namespace log;

const char * log::endl = "\n";
log::ostream	log::cout;

log::ostream& log::ostream::operator << (char *char_ptr) {
	printf("%s", char_ptr);
	return *this;
}

log::ostream& log::ostream::operator << (const char *char_ptr) {
	printf("%s", char_ptr);
	return *this;
}

log::ostream& log::ostream::operator << (int i) {
	printf("%d", i);
	return *this;
}

log::ostream& log::ostream::operator << (std::string str) {
	printf("%s", str.c_str());
	return *this;
}
