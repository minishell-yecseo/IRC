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

/*
log::ostream& operator << (log::ostream& out, const std::string& str) {
	pthread_mutex_lock(&print);
	printf("%s", str.c_str());
	pthread_mutex_unlock(&print);
	return *this;
}

std::ostream& operator << (std::ostream& out, std::string& str) {
	pthread_mutex_lock(&print);
	out.write(str.c_str(), str.size());
	pthread_mutex_unlock(&print);
	return *this;
}

std::ostream& operator << (std::ostream& out, char * char_ptr) {
	pthread_mutex_lock(&print);
	out.write(char_ptr, strlen(char_ptr));
	pthread_mutex_unlock(&print);
	return *this;
}

std::ostream& operator << (std::ostream& out, const int & i) {
	std::string	s;
	std::istringstream	iss(i);
	s = iss.str();
	out << s;
	return *this;
}

std::ostream& operator << (std::ostream& out,int & i) {
	std::string	s;
	std::istringstream	iss(i);
	s = iss.str();
	out << s;
	return *this;
}
*/
