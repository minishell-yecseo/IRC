#include "TestJoinCommand.hpp"

TestJoinCommand::TestJoinCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== JOINCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestJoinCommand::SetUp(void) {

}

void	TestJoinCommand::RunTest(void) {
}

void	TestJoinCommand::TearDown(void) {

}