#ifndef TESTCOMMAND_HPP
# define TESTCOMMAND_HPP

#include "Response.hpp"
#include "Command.hpp"
#include "TestUtils.hpp"

class TestCommand {
	public:
		TestCommand(Server *s, Client *c);
		const std::string& RunAndReturnRespInTest(Command *command);
		virtual void	SetUp(void) = 0;
		virtual void	RunTest(void) = 0;
		virtual void	TearDown(void) = 0;
		virtual ~TestCommand(void);

    protected:
		std::vector<std::string>	token_list_;
        Server      				*dummy_server_;
        Client      				*dummy_client_;
};
#endif
