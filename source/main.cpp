#include "ircserv.h"
#include <arpa/inet.h>
#include "log.hpp"

Mutex print;

void leaks();
int main(int argc, char **argv) {
//	atexit(leaks);
	print.init(NULL);

	Server server(argc, argv);
	server.Run();

	return 0;
}

void	leaks() {
	system("leaks --list -- $PPID");
}
