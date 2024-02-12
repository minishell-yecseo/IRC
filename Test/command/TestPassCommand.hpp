#ifndef TESTPASSCOMMAND_HPP
# define TESTPASSCOMMAND_HPP

#include "TestCommand.hpp"
#include "PassCommand.hpp"

class TestPassCommand: public TestCommand {
	public:
		TestPassCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	TearDown(void);
};
#endif
