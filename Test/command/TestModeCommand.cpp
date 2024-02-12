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
	this->dummy_server_->AddClientInTest(this->dummy_client_->get_sock(), *(this->dummy_client_));
	ModeCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	IsEqual("461 MODE :Not enough parameters", com.RunAndReturnRespInTest());

	this->token_list_.push_back("dummy_param");
	ModeCommand com2(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("324 :Not given modestring", com2.RunAndReturnRespInTest());

	this->token_list_.clear();
	this->token_list_.push_back("MODE");
	this->token_list_.push_back("#dummy");
	this->token_list_.push_back("+unvalidmodestr");
	ModeCommand com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("472 +unvalidmodestr :is unknown mode char to me", com3.RunAndReturnRespInTest());

	this->token_list_.clear();
	this->token_list_.push_back("MODE");
	this->token_list_.push_back("#dummy");
	this->token_list_.push_back("+kl-o");
	this->token_list_.push_back("dummy_param");
	ModeCommand com4(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("461 :No match param count", com4.RunAndReturnRespInTest());

	this->token_list_.clear();
	this->token_list_.push_back("MODE");
	this->token_list_.push_back("#dummy");
	this->token_list_.push_back("+t");
	ModeCommand com5(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual("403 #dummy :No such channel", com5.RunAndReturnRespInTest());

	this->dummy_server_->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	this->dummy_server_->AddChannelInTest("#dummy", dummy_channel);
	IsEqual("442 #dummy :You're not on that channel", com5.RunAndReturnRespInTest());

	Channel *ch_ptr;
	ch_ptr = this->dummy_server_->get_channel_ptr("#dummy");
	ch_ptr->Join(this->dummy_client_->get_sock(), ' ');
	IsEqual("482 #dummy :You're not channel operator", com5.RunAndReturnRespInTest());

	ch_ptr->Mode(this->dummy_client_->get_sock(), '@');
	IsEqual(":wooseoki MODE #dummy +t", com5.RunAndReturnRespInTest());
}

void	TestModeCommand::TearDown(void) {
	this->dummy_server_->DeleteClientInTest(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteChannelMutex("#dummy");
	this->dummy_server_->DeleteChannelInTest("#dummy");
	this->dummy_client_->UnsetAuthFlagInTest();
}