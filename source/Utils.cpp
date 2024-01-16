#include "Utils.hpp"

void error_handling(const char *message) {
	size_t len = strlen(message);
	if (write(STDERR_FILENO, message, len) == -1)
		exit(ERR_WRITE);
	exit(ERR_IRCSERV);
}

void error_handling(std::string message) {
	error_handling((const char *)message.c_str());
}
