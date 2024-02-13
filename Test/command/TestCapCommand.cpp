#include "TestCapCommand.hpp"

TestCapCommand::TestCapCommand(Server *s, Client *c) : TestCommand(s, c) {
	std::cout << "====== CAPCOMMAND ======\n";
	
	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestCapCommand::SetUp(void) {
	;
}

void	TestCapCommand::RunTest(void) {
	this->token_list_.push_back("CAP");
	CapCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("461 CAP :Not enough parameters", RunAndReturnRespInTest(&com));

	this->token_list_.push_back("LS");
	CapCommand comm(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("CAP * LS :", RunAndReturnRespInTest(&comm));
}

void	TestCapCommand::TearDown(void) {
	this->dummy_client_->UnsetAuthFlagInTest();
}