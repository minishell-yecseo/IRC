#include "TestResponse.hpp"
#include "TestSpecifyCommand.hpp"

void	TestResponse(Server *s, Client *c) {
	std::cout << YELLOW << "RESPONSE TEST START\n" << RESET;
	TestPingCommand(s, c);
	TestPartCommand(s, c);
	TestCapCommand(s, c);
	TestWhoisCommand(s, c);
	TestQuitCommand(s, c);
	TestTopicCommand(s, c);
	TestPrivmsgCommand(s, c);
	TestKickCommand(s, c);
	TestModeCommand(s, c);
	TestInviteCommand(s, c);
	TestUnvalidCommand(s, c);
	TestNickCommand(s, c);
	TestPassCommand(s, c);
	TestUserCommand(s, c);
	TestJoinCommand(s, c);
	std::cout << RED << "TEST FAILED: " << fail_count << RESET;
	std::cout << GREEN << " TEST SUCCESED: " << success_count << "\n" << RESET;
}
