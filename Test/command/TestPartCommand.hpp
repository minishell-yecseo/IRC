#ifndef TESTPARTCOMMAND_HPP
# define TESTPARTCOMMAND_HPP

#include "TestCommand.hpp"
#include "PartCommand.hpp"

class TestPartCommand: public TestCommand {
	public:
		TestPartCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	TearDown(void);
};
#endif
