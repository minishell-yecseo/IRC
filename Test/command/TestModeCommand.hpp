#ifndef TESTMODECOMMAND_HPP
# define TESTMODECOMMAND_HPP

#include "TestCommand.hpp"
#include "ModeCommand.hpp"

class TestModeCommand: public TestCommand {
	public:
		TestModeCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	RunFunctionTest(void);
		void	TearDown(void);
};
#endif
