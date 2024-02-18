#ifndef TESTQUITCOMMAND_HPP
# define TESTQUITCOMMAND_HPP

#include "TestCommand.hpp"
#include "QuitCommand.hpp"

class TestQuitCommand: public TestCommand {
	public:
		TestQuitCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	TearDown(void);

		Client	*new_dummy_client_;
};
#endif
