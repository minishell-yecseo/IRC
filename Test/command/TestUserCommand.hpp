#ifndef TESTUSERCOMMAND_HPP
# define TESTUSERCOMMAND_HPP

#include "TestCommand.hpp"
#include "UserCommand.hpp"

class TestUserCommand: public TestCommand {
	public:
		TestUserCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	TearDown(void);
};
#endif
