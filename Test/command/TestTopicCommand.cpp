#include "TestTopicCommand.hpp"

TestTopicCommand::TestTopicCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== TOPICCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestTopicCommand::SetUp(void) {
	;
}

void	TestTopicCommand::RunTest(void) {
	this->token_list_.push_back("TOPIC");
	TopicCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	this->dummy_server_->AddClientInTest(this->dummy_client_->get_sock(), *(this->dummy_client_));
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	IsEqual("461 TOPIC :Not enough parameters", com.RunAndReturnRespInTest());

	this->token_list_.clear();
	this->token_list_.push_back("TOPIC");
	this->token_list_.push_back("#empty");
	TopicCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("331 :No topic is set", com2.RunAndReturnRespInTest());

	this->token_list_.clear();
	this->token_list_.push_back("TOPIC");
	this->token_list_.push_back("#empty");
	this->token_list_.push_back("dummy_topic");
	TopicCommand com5(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("403 :No such channel", com5.RunAndReturnRespInTest());

	this->dummy_server_->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	this->dummy_server_->AddChannelInTest("#dummy", dummy_channel);
	this->token_list_.clear();
	this->token_list_.push_back("TOPIC");
	this->token_list_.push_back("#dummy");
	this->token_list_.push_back("dummy_topic");
	TopicCommand com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("442 #dummy :You're not on that channel", com3.RunAndReturnRespInTest());

	TopicCommand com4(this->token_list_, this->dummy_server_, this->dummy_client_);
	Channel *ch_ptr;
	ch_ptr = this->dummy_server_->get_channel_ptr("#dummy");
	ch_ptr->Join(this->dummy_client_->get_sock(), ' ');
	ch_ptr->set_mode(MODE_TOPIC, true);
	IsEqual("482 #dummy :You're not channel operator", com4.RunAndReturnRespInTest());
	ch_ptr->set_mode(MODE_TOPIC, false);
	IsEqual(":wooseoki TOPIC #dummy :dummy_topic", com4.RunAndReturnRespInTest());
}

void	TestTopicCommand::TearDown(void) {
	this->dummy_server_->DeleteClientInTest(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteChannelMutex("#dummy");
	this->dummy_server_->DeleteChannelInTest("#dummy");
	this->dummy_client_->UnsetAuthFlagInTest();
}