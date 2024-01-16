#include "ircserv.h"

void leaks();
int main(int argc, char **argv)
{
	atexit(leaks);
	Server server(argc, argv);
	server.run();
	return 0;
}

void	leaks()
{
	system("leaks $PPID");
}
