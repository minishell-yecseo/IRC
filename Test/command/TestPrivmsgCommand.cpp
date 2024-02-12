#include "TestPrivmsgCommand.hpp"

TestPrivmsgCommand::TestPrivmsgCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== PRIVMSGCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestPrivmsgCommand::SetUp(void) {
	new_dummy_client_.set_sock(8);
	new_dummy_client_.set_nick("saseo");
}

void	TestPrivmsgCommand::RunTest(void) {
	this->token_list_.push_back("PRIVMSG");
	this->dummy_server_->AddClientInTest(this->dummy_client_->get_sock(), *this->dummy_client_);
	PrivmsgCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	IsEqual("412 :No text to send", com.RunAndReturnRespInTest());

	this->token_list_.clear();
	this->token_list_.push_back("PRIVMSG");
	this->token_list_.push_back("#empty");
	PrivmsgCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("403 :No such channel", com2.RunAndReturnRespInTest());

	this->dummy_server_->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	this->dummy_server_->AddChannelInTest("#dummy", dummy_channel);
	this->token_list_.clear();
	this->token_list_.push_back("PRIVMSG");
	this->token_list_.push_back("#dummy");
	this->token_list_.push_back("dummy_msg");
	PrivmsgCommand com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("404 #dummy :Can not send to chanel", com3.RunAndReturnRespInTest());

	PrivmsgCommand com4(this->token_list_, this->dummy_server_, this->dummy_client_);
	Channel *ch_ptr;
	ch_ptr = this->dummy_server_->get_channel_ptr("#dummy");
	ch_ptr->Join(this->dummy_client_->get_sock(), ' ');
	IsEqual("411 :No recepient given", com4.RunAndReturnRespInTest());
	ch_ptr->Join(new_dummy_client_.get_sock(), ' ');
	IsEqual(":wooseoki PRIVMSG #dummy :dummy_msg", com4.RunAndReturnRespInTest());

	this->token_list_.clear();
	this->token_list_.push_back("PRIVMSG");
	this->token_list_.push_back("saseo");
	this->token_list_.push_back("dummy_msg");
	PrivmsgCommand com5(this->token_list_, this->dummy_server_, this->dummy_client_);
	this->dummy_server_->AddClientInTest(new_dummy_client_.get_sock(), new_dummy_client_);
	IsEqual(":wooseoki PRIVMSG saseo :dummy_msg", com5.RunAndReturnRespInTest());
}

void	TestPrivmsgCommand::TearDown(void) {
	this->dummy_server_->DeleteClientInTest(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteClientInTest(new_dummy_client_.get_sock());
	this->dummy_server_->DeleteChannelMutex("#dummy");
	this->dummy_server_->DeleteChannelInTest("#dummy");
	this->dummy_client_->UnsetAuthFlagInTest();
}