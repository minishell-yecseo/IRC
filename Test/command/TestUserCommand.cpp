#include "TestUserCommand.hpp"

TestUserCommand::TestUserCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== USERCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestUserCommand::SetUp(void) {
	;
}

void	TestUserCommand::RunTest(void) {
	AddClient(this->dummy_client_);
	
	this->token_list_.push_back("USER");
	UserCommand	com_no_param(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("461 USER :Not enough parameters", RunAndReturnRespInTest(&com_no_param));

	this->token_list_.push_back("white space");
	UserCommand	com_less_param(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("461 USER :Not enough parameters", RunAndReturnRespInTest(&com_less_param));

	this->token_list_.push_back("hostname");
	UserCommand	com_less_param2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("461 USER :Not enough parameters", RunAndReturnRespInTest(&com_less_param2));

	this->token_list_.push_back("servername");
	UserCommand	com_less_param3(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("461 USER :Not enough parameters", RunAndReturnRespInTest(&com_less_param3));

	this->token_list_.push_back("real name");
	UserCommand	com_white_space(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("400 :username must not has whitespace", \
			RunAndReturnRespInTest(&com_white_space));

	DeleteClient(this->dummy_client_->get_sock());
	this->token_list_.clear();
	this->dummy_client_->SetAuthFlag(AUTH);
	this->token_list_.push_back("USER");
	this->token_list_.push_back("test");
	this->token_list_.push_back("test");
	this->token_list_.push_back("test");
	this->token_list_.push_back("test test test");
	AddClient(this->dummy_client_);

	UserCommand	com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("462 USER :already registered", RunAndReturnRespInTest(&com));
	DeleteClient(this->dummy_client_->get_sock());

	this->dummy_client_->UnsetAuthFlagInTest();
	AddClient(this->dummy_client_);
	UserCommand	com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("", RunAndReturnRespInTest(&com2));
	DeleteClient(this->dummy_client_->get_sock());
}

void	TestUserCommand::TearDown(void) {
}
