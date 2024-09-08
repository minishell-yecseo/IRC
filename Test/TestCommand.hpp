#ifndef TESTCOMMAND_HPP
# define TESTCOMMAND_HPP

#include <assert.h>

#include "Response.hpp"
#include "Command.hpp"
#include "TestUtils.hpp"

class TestCommand {
	public:
		TestCommand(Server *s, Client *c);
		const std::string& RunAndReturnRespInTest(Command *command);
		virtual void	SetUp(void) = 0;
		virtual void	RunTest(void) = 0;
		virtual void	TearDown(void) = 0;
		virtual ~TestCommand(void);

    protected:
		bool	LockClientMutex(const int& sock);
		void	UnlockClientMutex(const int& sock);
		bool	AddChannelMutex(const std::string& name);
		bool	DeleteChannelMutex(const std::string& name);
		bool	LockChannelMutex(const std::string& name);
		void	UnlockChannelMutex(const std::string& name);
		bool	LockChannelListMutex(void);
		void	UnlockChannelListMutex(void);

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

		std::vector<std::string>	token_list_;
        Server      				*dummy_server_;
        Client      				*dummy_client_;
};
#endif
