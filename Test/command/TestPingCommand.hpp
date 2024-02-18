#ifndef TESTPINGCOMMAND_HPP
# define TESTPINGCOMMAND_HPP

#include "TestCommand.hpp"
#include "PingCommand.hpp"

class TestPingCommand: public TestCommand {
	public:
		TestPingCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	TearDown(void);

		Client	*new_dummy_client_;
};
#endif
