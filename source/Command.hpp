#ifndef COMAND_HPP
#define COMAND_HPP

#include <iostream>
#include <vector>

class ThreadPool;
class Channel;
class Client;
class Server;

#include "ThreadPool.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Response.hpp"
#include "log.hpp"

class Command
{
	public:
		virtual ~Command(void);
		virtual void	Run(void) = 0;
		virtual std::string	AnyOfError(void) = 0;
		Command(const std::vector<std::string> &token);
		void set_server(Server *server);
		void set_client(Client *client);
		Server* get_server(void);
		Client* get_client(void);
		void	DisconnectClient(void);
		void	SendResponse(const int& sock, const std::string& str);
		bool	IsRegistered(const int& fd);
		void	AuthCheckReply(void);

	protected:
		std::string					prefix_;
		std::string					command_;
		std::vector<std::string>	params_;
		Server	*server_;
		Client	*client_;
		int		client_sock_;
};
#endif
