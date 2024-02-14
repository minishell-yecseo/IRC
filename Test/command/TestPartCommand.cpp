#include "TestPartCommand.hpp"

TestPartCommand::TestPartCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== PARTCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestPartCommand::SetUp(void) {
	;
}

void	TestPartCommand::RunTest(void) {
	this->token_list_.push_back("PART");
	this->dummy_server_->AddClient(this->dummy_client_);
	PartCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("451 :You have not registered", RunAndReturnRespInTest(&com));
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	IsEqual("461 PART :Not enough parameters", RunAndReturnRespInTest(&com));

	this->token_list_.clear();
	this->token_list_.push_back("PART");
	this->token_list_.push_back("#empty");
	PartCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("403 #empty :No such channel", RunAndReturnRespInTest(&com2));

	Channel dummy_channel("#dummy");
	this->dummy_server_->AddChannel(&dummy_channel);
	this->token_list_.clear();
	this->token_list_.push_back("PART");
	this->token_list_.push_back("#dummy");
	PartCommand com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("442 #dummy :You're not on that channel", RunAndReturnRespInTest(&com3));

	PartCommand com4(this->token_list_, this->dummy_server_, this->dummy_client_);
	dummy_channel.Join(this->dummy_client_->get_sock(), '@');
	dummy_channel.Join(DUMMY_CLIENT_SOCK, ' ');
	IsEqual(":wooseoki PART #dummy", RunAndReturnRespInTest(&com4));
}

void	TestPartCommand::TearDown(void) {
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteChannel("#dummy");
	this->dummy_client_->UnsetAuthFlagInTest();
}
