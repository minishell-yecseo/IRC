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
		const std::string&	RunAndReturnRespInTest(void);

	protected:
		Command(const std::vector<std::string> &token_list, Server *s, Client *c);
		virtual ~Command(void);
		virtual void	AnyOfError(void) = 0;
		void	AuthCheckReply(void);
		void	DisconnectClient(void);
		bool	IsRegistered(const int& fd);
		void	SendResponse(const int& sock, const std::string& str);

		int							client_sock_;
		bool						is_success_;
		Server						*server_;
		Client						*client_;
		Response					resp_;
		std::string					prefix_;
		std::string					command_;
		std::vector<std::string>	params_;

	private:
		virtual void	Run(void) = 0;

		friend class	ThreadPool;
};
#endif
