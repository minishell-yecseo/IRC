#include "TestPrivmsgCommand.hpp"

TestPrivmsgCommand::TestPrivmsgCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== PRIVMSGCOMMAND ======\n";
	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestPrivmsgCommand::SetUp(void) {
	this->new_dummy_client_ = new Client(DUMMY_CLIENT_SOCK);
	this->new_dummy_client_->set_nick("saseo");
}

void	TestPrivmsgCommand::RunTest(void) {
	this->token_list_.push_back("PRIVMSG");
	this->dummy_server_->AddClient(this->dummy_client_);
	PrivmsgCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("451 :You have not registered", RunAndReturnRespInTest(&com));
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	IsEqual("412 :No text to send", RunAndReturnRespInTest(&com));

	this->token_list_.clear();
	this->token_list_.push_back("PRIVMSG");
	this->token_list_.push_back("#empty");
	PrivmsgCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("403 :No such channel", RunAndReturnRespInTest(&com2));

	Channel dummy_channel("#dummy");
	this->dummy_server_->AddChannel(&dummy_channel);
	this->token_list_.clear();
	this->token_list_.push_back("PRIVMSG");
	this->token_list_.push_back("#dummy");
	this->token_list_.push_back("dummy_msg");
	PrivmsgCommand com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("404 #dummy :Can not send to chanel", RunAndReturnRespInTest(&com3));

	PrivmsgCommand com4(this->token_list_, this->dummy_server_, this->dummy_client_);
	Channel *ch_ptr;
	ch_ptr = this->dummy_server_->get_channel_ptr("#dummy");
	ch_ptr->Join(this->dummy_client_->get_sock(), ' ');
	IsEqual("411 :No recepient given", RunAndReturnRespInTest(&com4));
	ch_ptr->Join(this->new_dummy_client_->get_sock(), ' ');
	IsEqual(":wooseoki PRIVMSG #dummy :dummy_msg", RunAndReturnRespInTest(&com4));

	this->token_list_.clear();
	this->token_list_.push_back("PRIVMSG");
	this->token_list_.push_back("saseo");
	this->token_list_.push_back("dummy_msg");
	PrivmsgCommand com5(this->token_list_, this->dummy_server_, this->dummy_client_);
	this->dummy_server_->AddClient(this->new_dummy_client_);
	IsEqual(":wooseoki PRIVMSG saseo :dummy_msg", RunAndReturnRespInTest(&com5));
}

void	TestPrivmsgCommand::TearDown(void) {
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteClient(this->new_dummy_client_->get_sock());
	this->dummy_server_->DeleteChannel("#dummy");
	this->dummy_client_->UnsetAuthFlagInTest();
	delete this->new_dummy_client_;
}
