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
	this->dummy_server_->AddClient(this->dummy_client_);
	IsEqual("451 :You have not registered", RunAndReturnRespInTest(&com));
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	IsEqual("461 TOPIC :Not enough parameters", RunAndReturnRespInTest(&com));

	this->token_list_.clear();
	this->token_list_.push_back("TOPIC");
	this->token_list_.push_back("#empty");
	TopicCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("331 :No topic is set", RunAndReturnRespInTest(&com2));

	this->token_list_.clear();
	this->token_list_.push_back("TOPIC");
	this->token_list_.push_back("#empty");
	this->token_list_.push_back("dummy_topic");
	TopicCommand com5(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("403 :No such channel", RunAndReturnRespInTest(&com5));

	Channel dummy_channel("#dummy");
	dummy_channel.Join(DUMMY_CLIENT_SOCK, ' ');
	this->dummy_server_->AddChannel(&dummy_channel);
	this->token_list_.clear();
	this->token_list_.push_back("TOPIC");
	this->token_list_.push_back("#dummy");
	this->token_list_.push_back("dummy_topic");
	TopicCommand com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("442 #dummy :You're not on that channel", RunAndReturnRespInTest(&com3));

	TopicCommand com4(this->token_list_, this->dummy_server_, this->dummy_client_);
	dummy_channel.Join(this->dummy_client_->get_sock(), ' ');
	dummy_channel.set_mode(MODE_TOPIC, true);
	IsEqual("482 #dummy :You're not channel operator", RunAndReturnRespInTest(&com4));
	dummy_channel.set_mode(MODE_TOPIC, false);
	IsEqual(":wooseoki TOPIC #dummy :dummy_topic", RunAndReturnRespInTest(&com4));
}

void	TestTopicCommand::TearDown(void) {
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteChannel("#dummy");
	this->dummy_client_->UnsetAuthFlagInTest();
}
