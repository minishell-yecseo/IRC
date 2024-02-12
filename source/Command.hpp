#ifndef COMAND_HPP
#define COMAND_HPP

#include <cstring>
#include <iostream>
#include <vector>
#include <exception>

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
		Command(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	DisconnectClient(void);
		void	SendResponse(const int& sock, const std::string& str);
		bool	IsRegistered(const int& fd);
		void	AuthCheckReply(void);
		const std::string&	RunAndReturnRespInTest(void);

	protected:
		friend class	ThreadPool;

		virtual void	Run(void) = 0;
		virtual void	AnyOfError(void) = 0;

		Response					resp_;
		bool						is_success_;
		std::string					prefix_;
		std::string					command_;
		std::vector<std::string>	params_;
		Server	*server_;
		Client	*client_;
		int		client_sock_;
};
#endif
