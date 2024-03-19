#include "TestNickCommand.hpp"

TestNickCommand::TestNickCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== NICKCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->RunFunctionTest();
	this->TearDown();
}

void	TestNickCommand::SetUp(void) {
	this->new_dummy_client_ = new Client(DUMMY_CLIENT_SOCK);
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
	AddClient(this->dummy_client_);
	NickCommand	com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("431 :No nickname given", RunAndReturnRespInTest(&com));
	DeleteClient(this->dummy_client_->get_sock());

	this->dummy_client_->SetAuthFlag(AUTH);
	AddClient(this->dummy_client_);
	IsEqual("431 :No nickname given", RunAndReturnRespInTest(&com));
	DeleteClient(this->dummy_client_->get_sock());
	this->dummy_client_->UnsetAuthFlagInTest();
	this->token_list_.clear();

	std::string	dup_name = "dup";
	this->new_dummy_client_->set_nick(dup_name);
	AddClient(this->new_dummy_client_);
	this->token_list_.push_back("NICK");
	this->token_list_.push_back(dup_name);
	NickCommand	com2(this->token_list_, this->dummy_server_, this->dummy_client_);

	AddClient(this->dummy_client_);
	IsEqual("433 dup :Nickname is already in use", RunAndReturnRespInTest(&com2));
	DeleteClient(this->dummy_client_->get_sock());

	this->dummy_client_->SetAuthFlag(AUTH);
	AddClient(this->dummy_client_);
	IsEqual("433 dup :Nickname is already in use", RunAndReturnRespInTest(&com2));
	DeleteClient(this->dummy_client_->get_sock());
	DeleteClient(this->new_dummy_client_->get_sock());
	this->dummy_client_->UnsetAuthFlagInTest();

	this->token_list_.clear();
	this->token_list_.push_back("NICK");
	this->token_list_.push_back("#tt");
	NickCommand	com3(this->token_list_, this->dummy_server_, this->dummy_client_);

	AddClient(this->dummy_client_);
	IsEqual("432 #tt :Erroneus nickname", RunAndReturnRespInTest(&com3));
	DeleteClient(this->dummy_client_->get_sock());

	this->dummy_client_->SetAuthFlag(AUTH);
	AddClient(this->dummy_client_);
	IsEqual("432 #tt :Erroneus nickname", RunAndReturnRespInTest(&com3));
	DeleteClient(this->dummy_client_->get_sock());
	this->dummy_client_->UnsetAuthFlagInTest();

	this->token_list_.clear();
	this->token_list_.push_back("NICK");
	this->token_list_.push_back("nick");
	NickCommand com4(this->token_list_, this->dummy_server_, this->dummy_client_);
	AddClient(this->dummy_client_);

	IsEqual("", RunAndReturnRespInTest(&com4));
	DeleteClient(this->dummy_client_->get_sock());

	this->dummy_client_->SetAuthFlag(AUTH);
	AddClient(this->dummy_client_);
	this->token_list_.clear();
	this->token_list_.push_back("NICK");
	this->token_list_.push_back("new-nick");
	NickCommand com4_1(this->token_list_, this->dummy_server_, this->dummy_client_);

	std::cout << "case4) NICK valid_nick [Auth Client]\n";
	IsEqual(":nick NICK new-nick", RunAndReturnRespInTest(&com4_1));

	this->token_list_.clear();
	this->token_list_.push_back(":new-nick");
	this->token_list_.push_back("NICK");
	this->token_list_.push_back("new-nick2");
	NickCommand com4_2(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::cout << "case4) :prev_nick NICK valid_nick [Auth Client]\n";
	IsEqual(":new-nick NICK new-nick2", RunAndReturnRespInTest(&com4_2));
	DeleteClient(this->dummy_client_->get_sock());
}

void	TestNickCommand::TearDown(void) {
	this->dummy_client_->Init(TEST_CLIENT_SOCK);
	this->dummy_client_->set_nick(TEST_CLIENT_NICK);
	delete this->new_dummy_client_;
}
