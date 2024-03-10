#include "TestModeCommand.hpp"

TestModeCommand::TestModeCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== MODECOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->RunFunctionTest();
	this->TearDown();
}

void	TestModeCommand::SetUp(void) {
	;
}

void	TestModeCommand::RunFunctionTest(void) {
	this->token_list_.clear();
	this->token_list_.push_back("MODE");
	ModeCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);

	std::cout << "====== IsValidMode METHOD ======\n";
	assert(true == com.IsValidMode("+i"));
	assert(true == com.IsValidMode("-k"));
	assert(true == com.IsValidMode("+itlok"));
	assert(true == com.IsValidMode("+ito-ok"));
	assert(true == com.IsValidMode("+itlok-itlok"));
	assert(true == com.IsValidMode("++"));
	assert(false == com.IsValidMode("+abcdefghijklmn"));
	assert(false == com.IsValidMode("it"));
	assert(false == com.IsValidMode(""));
	std::cout << GREEN << "SUCCESS\n" << RESET;

	std::cout << "====== IsLimitNumber METHOD ======\n";
	assert(true == com.IsLimitNumber("12345678"));
	assert(true == com.IsLimitNumber("0123"));
	assert(true == com.IsLimitNumber("1000"));
	assert(true == com.IsLimitNumber("0"));
	assert(false == com.IsLimitNumber("-1"));
	assert(false == com.IsLimitNumber("01234567890"));
	assert(false == com.IsLimitNumber("+1001"));
	std::cout << GREEN << "SUCCESS\n" << RESET;

	std::cout << "====== CheckParamCount METHOD ======\n";
	assert(1 == com.CheckParamCount("+o"));
	assert(2 == com.CheckParamCount("+ok"));
	assert(3 == com.CheckParamCount("+ioklt"));
	assert(4 == com.CheckParamCount("+okl-o"));
	std::cout << GREEN << "SUCCESS\n" << RESET;
}

void	TestModeCommand::RunTest(void) {
	this->token_list_.push_back("MODE");
	AddClient(this->dummy_client_);
	ModeCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("451 :You have not registered", RunAndReturnRespInTest(&com));
	this->dummy_client_->SetAuthFlag(AUTH);
	IsEqual("461 MODE :Not enough parameters", RunAndReturnRespInTest(&com));

	this->token_list_.push_back("dummy_param");
	ModeCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("324 :Not given modestring", RunAndReturnRespInTest(&com2));

	this->token_list_.clear();
	this->token_list_.push_back("MODE");
	this->token_list_.push_back("#dummy");
	this->token_list_.push_back("+unvalidmodestr");
	ModeCommand com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("472 +unvalidmodestr :is unknown mode char to me", RunAndReturnRespInTest(&com3));

	this->token_list_.clear();
	this->token_list_.push_back("MODE");
	this->token_list_.push_back("#dummy");
	this->token_list_.push_back("+kl-o");
	this->token_list_.push_back("dummy_param");
	ModeCommand com4(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("461 :No match param count", RunAndReturnRespInTest(&com4));

	this->token_list_.clear();
	this->token_list_.push_back("MODE");
	this->token_list_.push_back("#dummy");
	this->token_list_.push_back("+t");
	ModeCommand com5(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("403 #dummy :No such channel", RunAndReturnRespInTest(&com5));

	Channel dummy_channel("#dummy");
	AddChannel(&dummy_channel);
	IsEqual("442 #dummy :You're not on that channel", RunAndReturnRespInTest(&com5));

	Channel *ch_ptr;
	ch_ptr = get_channel_ptr("#dummy");
	ch_ptr->Join(this->dummy_client_->get_sock(), ' ');
	IsEqual("482 #dummy :You're not channel operator", RunAndReturnRespInTest(&com5));

	ch_ptr->Mode(this->dummy_client_->get_sock(), '@');
	IsEqual(":wooseoki MODE #dummy +t", RunAndReturnRespInTest(&com5));
}

void	TestModeCommand::TearDown(void) {
	DeleteClient(this->dummy_client_->get_sock());
	DeleteChannel("#dummy");
	this->dummy_client_->UnsetAuthFlagInTest();
}
