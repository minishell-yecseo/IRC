#ifndef TESTUNVALIDCOMMAND_HPP
# define TESTUNVALIDCOMMAND_HPP

#include "TestCommand.hpp"
#include "UnvalidCommand.hpp"

class TestUnvalidCommand: public TestCommand {
	public:
		TestUnvalidCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	TearDown(void);
};
#endif
