#include "TestKickCommand.hpp"

TestKickCommand::TestKickCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== KICKCOMMAND ======\n";
	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestKickCommand::SetUp(void) {
	this->new_dummy_client_ = new Client(DUMMY_CLIENT_SOCK);
	this->new_dummy_client_->set_nick("saseo");
}

void	TestKickCommand::RunTest(void) {
	this->token_list_.push_back("KICK");
	this->dummy_server_->AddClient(this->dummy_client_);
	KickCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("461 KICK :Not enough parameters", RunAndReturnRespInTest(&com));

	this->token_list_.push_back("#dummy");
	this->token_list_.push_back("saseo");
	KickCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("401 saseo :No such nick", RunAndReturnRespInTest(&com2));

	KickCommand com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	this->dummy_server_->AddClient(this->new_dummy_client_);
	IsEqual("403 #dummy :No such channel", RunAndReturnRespInTest(&com3));

	Channel dummy_channel("#dummy");
	this->dummy_server_->AddChannel(&dummy_channel);
	IsEqual("442 #dummy :You're not on that channel", RunAndReturnRespInTest(&com3));

	Channel *ch_ptr;
	ch_ptr = this->dummy_server_->get_channel_ptr("#dummy");
	ch_ptr->Join(this->dummy_client_->get_sock(), ' ');
	IsEqual("441 saseo #dummy :They aren't on the channel", RunAndReturnRespInTest(&com3));

	ch_ptr->Join(this->new_dummy_client_->get_sock(), ' ');
	IsEqual("482 #dummy :You're not channel operator", RunAndReturnRespInTest(&com3));

	ch_ptr->Mode(this->dummy_client_->get_sock(), '@');
	IsEqual(":wooseoki KICK #dummy saseo", RunAndReturnRespInTest(&com3));
}

void	TestKickCommand::TearDown(void) {
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteClient(this->new_dummy_client_->get_sock());
	this->dummy_server_->DeleteChannel("#dummy");
	this->dummy_client_->UnsetAuthFlagInTest();
	delete this->new_dummy_client_;
}
