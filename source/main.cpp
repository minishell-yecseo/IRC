#include "ircserv.h"

int main(int argc, char **argv)
{
	Server server(argc, argv);
	server.run();
	return 0;
}
