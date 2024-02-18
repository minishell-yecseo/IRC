#ifndef TESTNICKCOMMAND_HPP
# define TESTNICKCOMMAND_HPP

#include "TestCommand.hpp"
#include "NickCommand.hpp"

class TestNickCommand: public TestCommand {
	public:
		TestNickCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	RunFunctionTest(void);
		void	TearDown(void);
		Client	*new_dummy_client_;
};
#endif
