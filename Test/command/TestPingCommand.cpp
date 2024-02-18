#include "TestPingCommand.hpp"

TestPingCommand::TestPingCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== PINGCOMMAND ======\n";
	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestPingCommand::SetUp(void) {
	this->new_dummy_client_ = new Client(DUMMY_CLIENT_SOCK);
}

void	TestPingCommand::RunTest(void) {
	this->token_list_.push_back("PING");
	PingCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("451 :You have not registered", RunAndReturnRespInTest(&com));
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	IsEqual("461 PING :Not enough parameters", RunAndReturnRespInTest(&com));

	this->token_list_.clear();
	this->token_list_.push_back("PING");
	this->token_list_.push_back("localhost");
	PingCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("PONG localhost", RunAndReturnRespInTest(&com2));
}

void	TestPingCommand::TearDown(void) {
	this->dummy_client_->UnsetAuthFlagInTest();
	delete this->new_dummy_client_;
}
