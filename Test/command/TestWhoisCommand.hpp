#ifndef TESTWHOISCOMMAND_HPP
# define TESTWHOISCOMMAND_HPP

#include "TestCommand.hpp"
#include "WhoisCommand.hpp"

class TestWhoisCommand: public TestCommand {
	public:
		TestWhoisCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	TearDown(void);
};
#endif
