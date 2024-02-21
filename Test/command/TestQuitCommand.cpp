#include "TestQuitCommand.hpp"

TestQuitCommand::TestQuitCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== QUITCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->RunFunctionTest();
	this->TearDown();
}

void	TestQuitCommand::SetUp(void) {
	this->new_dummy_client_ = new Client(DUMMY_CLIENT_SOCK);
}

void	TestQuitCommand::RunFunctionTest(void) {
	std::cout << "====== NoticeQuit METHOD ======\n";

	AddClient(this->dummy_client_);
	AddClient(this->new_dummy_client_);
	Channel	dummy_channel("#TEST");
	dummy_channel.Join(this->dummy_client_->get_sock(), ' ');
	dummy_channel.Join(this->new_dummy_client_->get_sock(), ' ');
	AddChannel(&dummy_channel);

	this->token_list_.push_back("reason for quit");
	QuitCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string expect = " QUIT :reason for quit";
	com.NoticeQuit();
	assert(expect.compare(com.resp_.get_str()) == 0);

	this->token_list_.pop_back();
	QuitCommand com2(this->token_list_, this->dummy_server_, this->new_dummy_client_);
	std::string expect2 = " QUIT :";
	com2.NoticeQuit();
	assert(expect2.compare(com2.resp_.get_str()) == 0);

	DeleteClient(this->dummy_client_->get_sock());
	DeleteClient(this->new_dummy_client_->get_sock());
	DeleteChannel(dummy_channel.get_name());

	std::cout << GREEN << "SUCCESS\n" << RESET;
}

void	TestQuitCommand::RunTest(void) {
	this->token_list_.push_back("QUIT");
	AddClient(this->dummy_client_);
	QuitCommand com(this->token_list_, this->dummy_server_, this->dummy_client_);
	IsEqual(":wooseoki QUIT :Quit: Bye for now!", RunAndReturnRespInTest(&com));
	DeleteClient(this->dummy_client_->get_sock());
}

void	TestQuitCommand::TearDown(void) {
	delete this->new_dummy_client_;
}
