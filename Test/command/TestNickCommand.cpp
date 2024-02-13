#include "TestNickCommand.hpp"

TestNickCommand::TestNickCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== NICKCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->RunFunctionTest();
	this->TearDown();
}

void	TestNickCommand::SetUp(void) {

}

void	TestNickCommand::RunFunctionTest(void) {
	this->token_list_.clear();
	this->token_list_.push_back("NICK");
	NickCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	
	std::cout << "====== IsValidNick METHOD ======\n";
	assert(false == com.IsValidNick(""));
	assert(false == com.IsValidNick("1digit_s"));
	assert(false == com.IsValidNick("1_nick"));
	assert(false == com.IsValidNick("lengthovernine"));
	assert(false == com.IsValidNick("#nick"));
	assert(false == com.IsValidNick(":nick"));
	assert(false == com.IsValidNick("special_"));
	assert(false == com.IsValidNick("(special)"));
	assert(true == com.IsValidNick("nick[1]"));
	assert(true == com.IsValidNick("nick-123"));
	assert(true == com.IsValidNick("a1-b`2c^3"));
	assert(true == com.IsValidNick("a\\{nick}"));
	std::cout << GREEN << "SUCCESS\n" << RESET;
}

void	TestNickCommand::RunTest(void) {
	this->token_list_.push_back("NICK");
	this->dummy_server_->AddClient(this->dummy_client_);
	NickCommand	com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("431 :No nickname given", com.RunAndReturnRespInTest());
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());

	this->dummy_client_->SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(this->dummy_client_);
	IsEqual("431 :No nickname given", com.RunAndReturnRespInTest());
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_client_->UnsetAuthFlagInTest();
	this->token_list_.clear();

	Client	already_in_client;
	std::string	dup_name = "dup";
	already_in_client.set_nick(dup_name);
	already_in_client.set_sock(100);
	this->dummy_server_->AddClient(&already_in_client);
	this->token_list_.push_back("NICK");
	this->token_list_.push_back(dup_name);
	NickCommand	com2(this->token_list_, this->dummy_server_, this->dummy_client_);

	this->dummy_server_->AddClient(this->dummy_client_);
	IsEqual("433 dup :Nickname is already in use", com2.RunAndReturnRespInTest());
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());

	this->dummy_client_->SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(this->dummy_client_);
	IsEqual("433 dup :Nickname is already in use", com2.RunAndReturnRespInTest());
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteClient(already_in_client.get_sock());
	this->dummy_client_->UnsetAuthFlagInTest();

	this->token_list_.clear();
	this->token_list_.push_back("NICK");
	this->token_list_.push_back("#tt");
	NickCommand	com3(this->token_list_, this->dummy_server_, this->dummy_client_);

	this->dummy_server_->AddClient(this->dummy_client_);
	IsEqual("432 #tt :Erroneus nickname", com3.RunAndReturnRespInTest());
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());

	this->dummy_client_->SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(this->dummy_client_);
	IsEqual("432 #tt :Erroneus nickname", com3.RunAndReturnRespInTest());
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_client_->UnsetAuthFlagInTest();

	this->token_list_.clear();
	this->token_list_.push_back("NICK");
	this->token_list_.push_back("nick");
	NickCommand com4(this->token_list_, this->dummy_server_, this->dummy_client_);
	this->dummy_server_->AddClient(this->dummy_client_);

	IsEqual("", com4.RunAndReturnRespInTest());
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());

	this->dummy_client_->SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(this->dummy_client_);
	this->token_list_.clear();
	this->token_list_.push_back("NICK");
	this->token_list_.push_back("new-nick");
	NickCommand com4_1(this->token_list_, this->dummy_server_, this->dummy_client_);

	std::cout << "case4) NICK valid_nick [Auth Client]\n";
	IsEqual(":nick NICK new-nick", com4_1.RunAndReturnRespInTest());

	this->token_list_.clear();
	this->token_list_.push_back(":nick");
	this->token_list_.push_back("NICK");
	this->token_list_.push_back("new-nick2");
	NickCommand com4_2(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::cout << "case4) :prev_nick NICK valid_nick [Auth Client]\n";
	IsEqual(":nick NICK new-nick2", com4_2.RunAndReturnRespInTest());
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
}

void	TestNickCommand::TearDown(void) {

}