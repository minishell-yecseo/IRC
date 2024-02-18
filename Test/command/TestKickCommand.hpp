#ifndef TESTKICKCOMMAND_HPP
# define TESTKICKCOMMAND_HPP

#include "TestCommand.hpp"
#include "KickCommand.hpp"

class TestKickCommand: public TestCommand {
	public:
		TestKickCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	TearDown(void);

		Client	*new_dummy_client_;
};
#endif
