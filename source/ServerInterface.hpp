#ifndef SERVERINTERFACE_HPP
# define SERVERINTERFACE_HPP

#include "Channel.hpp"
#include "Client.hpp"

class ServerInterface {
    protected:
		virtual const std::string&	get_name(void) = 0;
		virtual const std::string&	get_version(void) = 0;
		virtual const std::string&	get_create_time(void) = 0;
		virtual std::string			set_create_time(void) = 0;

		/* mutex list functions */
		virtual bool	LockClientMutex(const int& sock) = 0;
		virtual void	UnlockClientMutex(const int& sock) = 0;

		virtual bool	LockChannelMutex(const std::string& name) = 0;
		virtual void	UnlockChannelMutex(const std::string& name) = 0;
		virtual bool	LockChannelListMutex(void) = 0;
		virtual void	UnlockChannelListMutex(void) = 0;
		virtual bool	AddChannelMutex(const std::string& name) = 0;
		virtual bool	DeleteChannelMutex(const std::string& name) = 0;

		/* for command process */
		virtual std::string		SearchClientBySock(const int& sock) = 0;
		virtual int		SearchClientByNick(const std::string& nick) = 0;
		virtual bool	SearchChannelByName(const std::string& name) = 0;
		virtual void	AddDeleteClient(const int& sock) = 0;
		virtual void	CreateChannel(const channel_info& info) = 0;
		virtual bool	AddChannel(Channel *channel) = 0;
		virtual bool	AddClient(Client *client) = 0;
        virtual void	CeaseChannel(const std::string& channel_name) = 0;
		virtual Channel	*DeleteChannel(const std::string& channel_name) = 0;
		virtual Client	*DeleteClient(const int& sock) = 0;
		virtual std::map<std::string, Channel*>& get_channels(void) = 0;
		virtual Channel	*get_channel_ptr(const std::string& name) = 0;

		/* Authentication */
		virtual bool	AuthPassword(const std::string& password) = 0;
        virtual ~ServerInterface(void) {};

		friend class	Command;
		friend class	TestCommand;
};
#endif