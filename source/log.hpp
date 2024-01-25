#ifndef LOG_HPP
#define LOG_HPP

#include <stdio.h>
#include <string>
#include "Mutex.hpp"

namespace log {
	extern const char *endl;
	class ostream {
		public:
			ostream& operator << (char * char_ptr);
			ostream& operator << (const char * char_ptr);
			ostream& operator << (int i);
			ostream& operator << (std::string str);
			ostream& operator << (pthread_t tid);
	};
	extern ostream cout;
}

#endif
