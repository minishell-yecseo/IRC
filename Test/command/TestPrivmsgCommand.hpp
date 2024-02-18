#ifndef TESTPRIVMSGCOMMAND_HPP
# define TESTPRIVMSGCOMMAND_HPP

#include "TestCommand.hpp"
#include "PrivmsgCommand.hpp"

class TestPrivmsgCommand: public TestCommand {
	public:
		TestPrivmsgCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	TearDown(void);

		Client	*new_dummy_client_;
};
#endif
