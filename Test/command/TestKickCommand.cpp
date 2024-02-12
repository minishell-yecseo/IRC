#include "TestKickCommand.hpp"

TestKickCommand::TestKickCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== KICKCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestKickCommand::SetUp(void) {
	new_dummy_client_.set_sock(8);
	new_dummy_client_.set_nick("saseo");
}

void	TestKickCommand::RunTest(void) {
	this->token_list_.push_back("KICK");
	this->dummy_server_->AddClientInTest(this->dummy_client_->get_sock(), *this->dummy_client_);
	KickCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("461 KICK :Not enough parameters", com.RunAndReturnRespInTest());

	this->token_list_.push_back("#dummy");
	this->token_list_.push_back("saseo");
	KickCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("401 saseo :No such nick", com2.RunAndReturnRespInTest());

	KickCommand com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	this->dummy_server_->AddClientInTest(new_dummy_client_.get_sock(), new_dummy_client_);
	IsEqual("403 #dummy :No such channel", com3.RunAndReturnRespInTest());

	this->dummy_server_->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	this->dummy_server_->AddChannelInTest("#dummy", dummy_channel);
	IsEqual("442 #dummy :You're not on that channel", com3.RunAndReturnRespInTest());

	Channel *ch_ptr;
	ch_ptr = this->dummy_server_->get_channel_ptr("#dummy");
	ch_ptr->Join(this->dummy_client_->get_sock(), ' ');
	IsEqual("441 saseo #dummy :They aren't on the channel", com3.RunAndReturnRespInTest());

	ch_ptr->Join(new_dummy_client_.get_sock(), ' ');
	IsEqual("482 #dummy :You're not channel operator", com3.RunAndReturnRespInTest());

	ch_ptr->Mode(this->dummy_client_->get_sock(), '@');
	IsEqual(":wooseoki KICK #dummy saseo", com3.RunAndReturnRespInTest());
}

void	TestKickCommand::TearDown(void) {
	this->dummy_server_->DeleteClientInTest(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteClientInTest(new_dummy_client_.get_sock());
	this->dummy_server_->DeleteChannelMutex("#dummy");
	this->dummy_server_->DeleteChannelInTest("#dummy");
	this->dummy_client_->UnsetAuthFlagInTest();

}