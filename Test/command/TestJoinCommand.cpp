#include "TestJoinCommand.hpp"

TestJoinCommand::TestJoinCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== JOINCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->TearDown();
}

void	TestJoinCommand::SetUp(void) {

}

void	TestJoinCommand::RunTest(void) {
	//1-1) create channel
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(this->dummy_client_);
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("#TEST");
	JoinCommand	com(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	exp = ":wooseoki!test@test JOIN #TEST\r\n353 wooseoki = #TEST :@wooseoki \r\n366 wooseoki #TEST\r\n";
	IsEqual(exp, com.RunAndReturnRespInTest());

	//1-2.0 defalut mode
	Client	dc2(100);
	dc2.SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(&dc2);
	JoinCommand	com2(this->token_list_, this->dummy_server_, &dc2);
	std::string	exp2 = ":unknown!user@host JOIN #TEST\r\n353 unknown = #TEST :@wooseoki unknown \r\n366 unknown #TEST\r\n";
	IsEqual(exp2, com2.RunAndReturnRespInTest());

	this->dummy_server_->DeleteClient(dc2.get_sock());
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_server_->CeaseChannel("#TEST");
	this->dummy_client_->UnsetAuthFlagInTest();
	this->token_list_.clear();

	//1-2.1 +i
	Client	dc3(101);
	dc3.set_nick("dc3");
	dc3.SetAuthFlag(FT_AUTH);
	Channel	ch3("#TEST");
	ch3.set_mode(MODE_INVITE, true);
	ch3.Invite(dc3.get_sock());
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(this->dummy_client_);
	this->dummy_server_->AddClient(&dc3);
	this->dummy_server_->AddChannel(ch3);
	
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("#TEST");
	JoinCommand	com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	JoinCommand	com3_1(this->token_list_, this->dummy_server_, &dc3);
	
	std::string	fail3 = "473 wooseoki #TEST : Cannot join channel (+i)";
	std::string	succ3 = ":dc3!user@host JOIN #TEST\r\n353 dc3 = #TEST :dc3 \r\n366 dc3 #TEST\r\n";
	IsEqual(fail3, com3.RunAndReturnRespInTest());
	IsEqual(succ3, com3_1.RunAndReturnRespInTest());
	this->token_list_.clear();
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteClient(dc3.get_sock());
	this->dummy_server_->CeaseChannel(ch3.get_name());

	//1-2.2 +k
	Client	dc4(102);
	dc4.SetAuthFlag(FT_AUTH);
	dc4.set_nick("dc4");

	Channel	ch4("#TEST-KEY");
	ch4.set_mode(MODE_KEY, true);
	ch4.set_password("succ_key");

	this->dummy_server_->AddClient(&dc4);
	this->dummy_server_->AddClient(this->dummy_client_);
	this->dummy_server_->AddChannel(ch4);

	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("#TEST-KEY");
	this->token_list_.push_back("fail_key");
	JoinCommand	com4_fail(this->token_list_, this->dummy_server_, this->dummy_client_);
	this->token_list_.pop_back();
	this->token_list_.push_back("succ_key");
	JoinCommand com4_succ(this->token_list_, this->dummy_server_, &dc4);
	std::string	fail4 = "475 wooseoki #TEST-KEY : Cannot join channel (+k)";
	std::string	succ4 = ":dc4!user@host JOIN #TEST-KEY\r\n353 dc4 = #TEST-KEY :dc4 \r\n366 dc4 #TEST-KEY\r\n";
	IsEqual(fail4, com4_fail.RunAndReturnRespInTest());
	IsEqual(succ4, com4_succ.RunAndReturnRespInTest());

	this->token_list_.clear();
	this->dummy_server_->DeleteClient(dc4.get_sock());
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_server_->CeaseChannel(ch4.get_name());

	//1-2.3 +t
	Channel	ch5("&TEST-TOPIC");
	ch5.set_mode(MODE_TOPIC, true);
	ch5.set_topic("topic!");

	this->dummy_server_->AddClient(this->dummy_client_);
	this->dummy_server_->AddChannel(ch5);

	this->token_list_.push_back("JOIN");
	this->token_list_.push_back(ch5.get_name());
	JoinCommand	com5(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	succ5 = ":wooseoki!test@test JOIN &TEST-TOPIC\r\n332 wooseoki &TEST-TOPIC :topic!\r\n";
	succ5 = succ5 + "353 wooseoki = &TEST-TOPIC :wooseoki \r\n366 wooseoki &TEST-TOPIC\r\n";
	IsEqual(succ5, com5.RunAndReturnRespInTest());

	this->token_list_.clear();
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_server_->CeaseChannel(ch5.get_name());

	// not registered client's join request
	this->dummy_client_->UnsetAuthFlagInTest();
	this->dummy_server_->AddClient(this->dummy_client_);
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("#TEST");
	JoinCommand	com6(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	fail6 = "400 wooseoki JOIN : Not registered in Server";
	IsEqual(fail6, com6.RunAndReturnRespInTest());
	this->token_list_.clear();
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());

	// join failure caused by channel number limit
	Channel	ch7("&TEST-LIMIT");
	ch7.set_limit(1);
	ch7.Join(-1, ' ');
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(this->dummy_client_);
	this->dummy_server_->AddChannel(ch7);
	token_list_.push_back("JOIN");
	token_list_.push_back("&TEST-LIMIT");
	JoinCommand	com7(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	fail7 = "471 wooseoki &TEST-LIMIT :Cannot join channel (+l)";
	IsEqual(fail7, com7.RunAndReturnRespInTest());
	this->token_list_.clear();
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_server_->DeleteChannel(ch7.get_name());
	this->dummy_client_->UnsetAuthFlagInTest();

	// join failure caused by already joined client's request
	Channel	ch8("&TEST-MEMBER");
	ch8.Join(this->dummy_client_->get_sock(), '@');
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(this->dummy_client_);
	this->dummy_server_->AddChannel(ch8);
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("&TEST-MEMBER");
	JoinCommand	com8(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	fail8 = "400 :wooseoki is already in &TEST-MEMBER";
	IsEqual(fail8, com8.RunAndReturnRespInTest());
	this->token_list_.clear();
	this->dummy_client_->UnsetAuthFlagInTest();
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
	this->dummy_server_->CeaseChannel(ch8.get_name());

	// join failure caused by bad channel mask
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(this->dummy_client_);
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("BAD");
	JoinCommand	com9(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	fail9 = "476 BAD :Bad Channel Mask";
	IsEqual(fail9, com9.RunAndReturnRespInTest());
	this->token_list_.clear();
	this->dummy_client_->UnsetAuthFlagInTest();
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());

	// join failure caused by bad channel name
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(this->dummy_client_);
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("#");
	JoinCommand	com10(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	fail10 = "403 wooseoki # :No such channel";
	IsEqual(fail10, com10.RunAndReturnRespInTest());
	this->token_list_.clear();
	this->dummy_client_->UnsetAuthFlagInTest();
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());

	// join fail with wrong key string
	this->dummy_client_->SetAuthFlag(FT_AUTH);
	this->dummy_server_->AddClient(this->dummy_client_);
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("#TEST");
	this->token_list_.push_back("key with whitespace");
	JoinCommand	com11(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	fail11 = "400 : key with whitespace";
	IsEqual(fail11, com11.RunAndReturnRespInTest());
	this->token_list_.clear();
	this->dummy_client_->UnsetAuthFlagInTest();
	this->dummy_server_->DeleteClient(this->dummy_client_->get_sock());
}

void	TestJoinCommand::TearDown(void) {

}
