#include "TestPassCommand.hpp"

TestPassCommand::TestPassCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== PASSCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestPassCommand::SetUp(void) {
	;
}

void	TestPassCommand::RunTest(void) {
	this->token_list_.push_back("PASS");
	PassCommand	com_no_param(this->token_list_, this->dummy_server_, this->dummy_client_);
	this->dummy_client_->UnsetAuthFlagInTest();
	this->dummy_server_->AddClient(this->dummy_client_);
	IsEqual("400 :parameter number error", RunAndReturnRespInTest(&com_no_param));
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());

	this->dummy_client_->SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(this->dummy_client_);
	IsEqual("451 PASS :already registered", RunAndReturnRespInTest(&com_no_param));
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());

	this->dummy_client_->UnsetAuthFlagInTest();
	this->dummy_server_->AddClient(this->dummy_client_);
	this->token_list_.push_back("wrong-password");
	PassCommand	com_wrong(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("464 :Password incorrect", RunAndReturnRespInTest(&com_wrong));
}

void	TestPassCommand::TearDown(void) {
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_client_->UnsetAuthFlagInTest();
}