#include "TestInviteCommand.hpp"

TestInviteCommand::TestInviteCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== INVITECOMMAND ======\n";
	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestInviteCommand::SetUp(void) {
	this->new_dummy_client_ = new Client(TEST_CLIENT_SOCK + 1);
	this->new_dummy_client_->set_nick("saseo");
}

void	TestInviteCommand::RunTest(void) {
	this->token_list_.push_back("INVITE");
	this->dummy_server_->AddClient(this->dummy_client_);
	InviteCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("451 :You have not registered", RunAndReturnRespInTest(&com));
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	IsEqual("461 INVITE :Not enough parameters", RunAndReturnRespInTest(&com));

	this->token_list_.push_back("saseo");
	this->token_list_.push_back("#dummy");
	InviteCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("401 saseo :No such nick", RunAndReturnRespInTest(&com2));

	this->dummy_server_->AddClient(this->new_dummy_client_);
	IsEqual("403 #dummy :No such channel", RunAndReturnRespInTest(&com2));

	Channel dummy_channel("#dummy");
	this->dummy_server_->AddChannel(&dummy_channel);
	IsEqual("442 #dummy :You're not on that channel", RunAndReturnRespInTest(&com2));

	Channel *ch_ptr;
	ch_ptr = this->dummy_server_->get_channel_ptr("#dummy");
	if (ch_ptr != NULL) {
		ch_ptr->Join(this->dummy_client_->get_sock(), ' ');
		IsEqual("482 #dummy :You're not channel operator", RunAndReturnRespInTest(&com2));
		ch_ptr->Mode(this->dummy_client_->get_sock(), '@');
		ch_ptr->Join(this->new_dummy_client_->get_sock(), ' ');
		IsEqual("443 saseo #dummy :is already on cahnnel", RunAndReturnRespInTest(&com2));
		ch_ptr->Kick(this->new_dummy_client_->get_sock());
		IsEqual(":wooseoki 341 saseo #dummy", RunAndReturnRespInTest(&com2));
	}
}

void	TestInviteCommand::TearDown(void) {
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteClient(this->new_dummy_client_->get_sock());
	this->dummy_server_->DeleteChannel("#dummy");
	this->dummy_client_->UnsetAuthFlagInTest();
	delete this->new_dummy_client_;
}
