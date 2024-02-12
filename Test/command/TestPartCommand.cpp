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
	this->dummy_server_->AddClientInTest(this->dummy_client_->get_sock(), *this->dummy_client_);
	PartCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	IsEqual("461 PART :Not enough parameters", com.RunAndReturnRespInTest());

	this->token_list_.clear();
	this->token_list_.push_back("PART");
	this->token_list_.push_back("#empty");
	PartCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("403 #empty :No such channel", com2.RunAndReturnRespInTest());

	this->dummy_server_->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	this->dummy_server_->AddChannelInTest("#dummy", dummy_channel);
	this->token_list_.clear();
	this->token_list_.push_back("PART");
	this->token_list_.push_back("#dummy");
	PartCommand com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("442 #dummy :You're not on that channel", com3.RunAndReturnRespInTest());

	PartCommand com4(this->token_list_, this->dummy_server_, this->dummy_client_);
	Channel *ch_ptr;
	ch_ptr = this->dummy_server_->get_channel_ptr("#dummy");
	ch_ptr->Join(this->dummy_client_->get_sock(), '@');
	IsEqual(":wooseoki PART #dummy", com4.RunAndReturnRespInTest());
}

void	TestPartCommand::TearDown(void) {
	this->dummy_server_->DeleteClientInTest(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteChannelMutex("#dummy");
	this->dummy_server_->DeleteChannelInTest("#dummy");
	this->dummy_client_->UnsetAuthFlagInTest();
}