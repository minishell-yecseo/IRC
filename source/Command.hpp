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
#include "ServerInterface.hpp"

class Command {
	protected:
		Command(const std::vector<std::string>& token_list, ServerInterface* s, Client* c);
		virtual ~Command(void);
		virtual void	AnyOfError(void) = 0;
		virtual void		Run(void) = 0;
		const std::string&	get_response(void);
		void	AuthCheckReply(void);
		void	DisconnectClient(void);
		bool	IsRegistered(const int& fd);
		void	SendResponse(const int& sock, const std::string& str);

		int							client_sock_;
		bool						is_success_;
		ServerInterface*			server_;
		Client*						client_;
		Response					resp_;
		std::string					prefix_;
		std::string					command_;
		std::vector<std::string>	params_;

		bool	LockClientMutex(const int& sock);
		void	UnlockClientMutex(const int& sock);

		bool	LockChannelMutex(const std::string& name);
		void	UnlockChannelMutex(const std::string& name);
		bool	LockChannelListMutex(void);
		void	UnlockChannelListMutex(void);
		bool	AddChannelMutex(const std::string& name);
		bool	DeleteChannelMutex(const std::string& name);

		std::string		SearchClientBySock(const int& sock);
		int		SearchClientByNick(const std::string& nick);
		bool	SearchChannelByName(const std::string& name);
		void	AddDeleteClient(const int& sock);
		void	CreateChannel(const channel_info& info);//allocate Channel
		bool	AddChannel(Channel *channel);
		bool	AddClient(Client *client);
		void	CeaseChannel(const std::string& channel_name);//free Channel
		Channel	*DeleteChannel(const std::string& channel_name);
		Client	*DeleteClient(const int& sock);
		std::map<std::string, Channel*>& get_channels(void);
		Channel	*get_channel_ptr(const std::string& name);

		bool	AuthPassword(const std::string& password);

	private:
		DISALLOW_COPY_AND_ASSIGN(Command);

 		friend class	ThreadPool;
		friend class	TestCommand;
};
#endif
