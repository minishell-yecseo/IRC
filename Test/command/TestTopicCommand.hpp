#ifndef TESTTOPICCOMMAND_HPP
# define TESTTOPICCOMMAND_HPP

#include "TestCommand.hpp"
#include "TopicCommand.hpp"

class TestTopicCommand: public TestCommand {
	public:
		TestTopicCommand(Server *s, Client *c);

	private:
		void	SetUp(void);
		void	RunTest(void);
		void	TearDown(void);
};
#endif
