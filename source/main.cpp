#include "ircserv.h"
#include <arpa/inet.h>
#include "log.hpp"

void leaks();
int main(int argc, char **argv) {
//	atexit(leaks);

	Server server(argc, argv);
	server.Run();

	return 0;
}

void	leaks() {
	system("leaks --list -- $PPID");
}
