#include "ircserv.h"
#include <arpa/inet.h>

void leaks();
int main(int argc, char **argv)
{
	atexit(leaks);
	Server server(argc, argv);
	struct sockaddr_in addr = server.get_addr();
	char *ip_addr = inet_ntoa(addr.sin_addr);
	std::cout << server.get_name() << ": started at " << ip_addr << " " << server.get_port() << "\n";
	server.Run();
	return 0;
}

void	leaks()
{
	system("leaks --list -- $PPID");
}
