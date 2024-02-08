#include "ircserv.h"
#include <arpa/inet.h>
#include "log.hpp"

Mutex print;

int main(int argc, char **argv) {
	print.init(NULL);

	Server server(argc, argv);
	server.Run();

	return 0;
}
