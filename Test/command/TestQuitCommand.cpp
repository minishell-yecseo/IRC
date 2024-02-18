#include "TestQuitCommand.hpp"

TestQuitCommand::TestQuitCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== QUITCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestQuitCommand::SetUp(void) {
	this->new_dummy_client_ = new Client(DUMMY_CLIENT_SOCK);
}

void	TestQuitCommand::RunTest(void) {
	this->token_list_.push_back("QUIT");
	this->dummy_server_->AddClient(this->dummy_client_);
	QuitCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual(":wooseoki QUIT :Quit: Bye for now!", RunAndReturnRespInTest(&com));
}

void	TestQuitCommand::TearDown(void) {
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	delete this->new_dummy_client_;
}
