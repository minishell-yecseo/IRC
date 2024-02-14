#ifndef TESTJOINCOMMAND_HPP
# define TESTJOINCOMMAND_HPP

#include "TestCommand.hpp"
#include "JoinCommand.hpp"

class TestJoinCommand: public TestCommand {
	public:
		TestJoinCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	RunFunctionTest(void);
		void	TearDown(void);
};
#endif
