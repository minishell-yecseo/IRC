#ifndef TESTINVITECOMMAND_HPP
# define TESTINVITECOMMAND_HPP

#include "TestCommand.hpp"
#include "InviteCommand.hpp"

class TestInviteCommand: public TestCommand {
	public:
		TestInviteCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	TearDown(void);

		Client	*new_dummy_client_;
};
#endif
