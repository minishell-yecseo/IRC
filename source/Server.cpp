#include "Server.hpp"

Server::Server(int argc, char **argv)
{
	if (argc != 3)
	{
		std::string error_message;
		std::string program_name = argv[0];
		error_message = "Usage: " + program_name + " <port> <password>\n";
		error_handling(error_message);
	}
	std::cout << "?\n";
}
