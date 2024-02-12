#ifndef TESTCAPCOMMAND_HPP
# define TESTCAPCOMMAND_HPP

#include "TestCommand.hpp"
#include "CapCommand.hpp"

class TestCapCommand: public TestCommand {
	public:
		TestCapCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	TearDown(void);
};
#endif
