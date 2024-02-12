#include "TestWhoisCommand.hpp"

TestWhoisCommand::TestWhoisCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== WHOISCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestWhoisCommand::SetUp(void) {
	;
}

void	TestWhoisCommand::RunTest(void) {
	this->token_list_.push_back("WHOIS");
	WhoisCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	IsEqual("431 :No nickname given", com.RunAndReturnRespInTest());

	this->token_list_.push_back("dummy_client");
	WhoisCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("401 dummy_client :No such nick", com2.RunAndReturnRespInTest());

	this->token_list_.clear();
	this->token_list_.push_back("WHOIS");
	this->token_list_.push_back(this->dummy_client_->get_nick());
	this->dummy_server_->AddClientInTest(this->dummy_client_->get_sock(), *(this->dummy_client_));
	WhoisCommand com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("318 WHOISEND", com3.RunAndReturnRespInTest());
}

void	TestWhoisCommand::TearDown(void) {
	this->dummy_server_->DeleteClientInTest(this->dummy_client_->get_sock());
	this->dummy_client_->UnsetAuthFlagInTest();
}