#include "TestUnvalidCommand.hpp"

TestUnvalidCommand::TestUnvalidCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== UNVALIDCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestUnvalidCommand::SetUp(void) {
	;
}

void	TestUnvalidCommand::RunTest(void) {
	this->token_list_.push_back("blahblah");
	UnvalidCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("421 blahblah :Unknown command", RunAndReturnRespInTest(&com));
}

void	TestUnvalidCommand::TearDown(void) {
	;
}