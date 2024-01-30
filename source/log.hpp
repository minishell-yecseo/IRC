#ifndef LOG_HPP
#define LOG_HPP

#include <pthread.h>
#include <stdio.h>
#include <string>

namespace log {
	extern const char *endl;
	class ostream {
		public:
			ostream& operator << (char * char_ptr);
			ostream& operator << (const char * char_ptr);
			ostream& operator << (int i);
			ostream& operator << (std::string str);
	};
	extern ostream cout;
}

#endif
