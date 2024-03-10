#include "TestJoinCommand.hpp"

TestJoinCommand::TestJoinCommand(Server *s, Client *c): TestCommand(s, c) {
	std::cout << "====== JOINCOMMAND ======\n";

	this->SetUp();
	this->RunTest();
	this->RunFunctionTest();
	this->TearDown();
}

void	TestJoinCommand::SetUp(void) {

}

void	TestJoinCommand::RunFunctionTest(void) {
	//	void	ParseParam(void);OOOOOOOOOOOOOOOOOOOOOOOOOOOOO
	//	bool	IsValidChannelInfo(const int& idx);OOOOOOOOOOO
	//	bool	IsChannelString(const std::string &str);OOOOOO
	//	bool	JoinErrorCheck(const channel_info& info);OOOOO
	{
		std::cout << "====== ParseParam METHOD ======\n";
		this->token_list_.clear();
		this->token_list_.push_back("JOIN");
		JoinCommand	com(this->token_list_, this->dummy_server_, this->dummy_client_);
		com.ParseParam();
		assert(com.channels_.size() == 0);
		assert(com.keys_.size() == 0);

		this->token_list_.push_back("#A,#B,&C,DD");
		JoinCommand	com1(this->token_list_, this->dummy_server_, this->dummy_client_);
		com1.ParseParam();
		assert(com1.channels_.size() == 4);
		assert(com1.channels_[0].compare("#A") == 0);
		assert(com1.channels_[1].compare("#B") == 0);
		assert(com1.channels_[2].compare("&C") == 0);
		assert(com1.channels_[3].compare("DD") == 0);
		assert(com1.keys_.size() == 0);

		this->token_list_.push_back("akey,x,ckey");
		JoinCommand	com2(this->token_list_, this->dummy_server_, this->dummy_client_);
		com2.ParseParam();
		assert(com2.channels_.size() == 4);
		assert(com2.channels_[0].compare("#A") == 0);
		assert(com2.channels_[1].compare("#B") == 0);
		assert(com2.channels_[2].compare("&C") == 0);
		assert(com2.channels_[3].compare("DD") == 0);
		assert(com2.keys_.size() == 3);
		assert(com2.keys_[0].compare("akey") == 0);
		assert(com2.keys_[1].compare("x") == 0);
		assert(com2.keys_[2].compare("ckey") == 0);

		std::cout << GREEN << "SUCCESS\n" << RESET;
	}
	
	{
		std::cout << "====== IsValidChannelInfo METHOD ======\n";
		this->token_list_.clear();
		this->token_list_.push_back("JOIN");
		JoinCommand	com(this->token_list_, this->dummy_server_, this->dummy_client_);
		com.ParseParam();
		assert(false == com.IsValidChannelInfo(0));

		this->token_list_.push_back("#TEST");
		JoinCommand	com2(this->token_list_, this->dummy_server_, this->dummy_client_);
		com2.ParseParam();
		assert(false == com2.IsValidChannelInfo(1));
		assert(true == com2.IsValidChannelInfo(0));

		this->token_list_.push_back("valid_key");
		JoinCommand	com3_valid(this->token_list_, this->dummy_server_, this->dummy_client_);
		com3_valid.ParseParam();
		this->token_list_[2] = "unvalid key";
		JoinCommand	com3_unvalid(this->token_list_, this->dummy_server_, this->dummy_client_);
		com3_unvalid.ParseParam();
		assert(true == com3_valid.IsValidChannelInfo(0));
		assert(false == com3_unvalid.IsValidChannelInfo(0));

		this->token_list_.clear();
		this->token_list_.push_back("JOIN");
		this->token_list_.push_back("A");
		JoinCommand	com4_bad_mask(this->token_list_, this->dummy_server_, this->dummy_client_);
		com4_bad_mask.ParseParam();
		assert(false == com4_bad_mask.IsValidChannelInfo(0));

		//201 character include channel type mask(#)
		this->token_list_[1] = "\
#aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		JoinCommand	com5_over_200(this->token_list_, this->dummy_server_, this->dummy_client_);
		com5_over_200.ParseParam();
		assert(false == com5_over_200.IsValidChannelInfo(0));
		std::cout << GREEN << "SUCCESS\n" << RESET;
		this->token_list_.clear();
	}

	{
		std::cout << "====== IsChannelString METHOD ======\n";
		std::string	valid_str = "#CC";
		std::string error_strs[8];
	
		error_strs[0] = "";
		error_strs[1] = "\
#aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		for (size_t i = 2; i < sizeof(error_strs) / sizeof(std::string); ++i) {
			error_strs[i].resize(2);
			error_strs[i][0] = 'T';
		}
		error_strs[2][1] = 0;
		error_strs[2][1] = 7;
		error_strs[2][1] = 20;
		error_strs[2][1] = 12;
		error_strs[2][1] = 15;
		error_strs[2][1] = ',';

		this->token_list_.push_back("JOIN");
		JoinCommand	dummy_com(this->token_list_, this->dummy_server_, this->dummy_client_);
		for (size_t i = 0; i < sizeof(error_strs) / sizeof(std::string); ++i) {
			assert(false == dummy_com.IsChannelString(error_strs[i]));
		}
		assert(true == dummy_com.IsChannelString(valid_str));
		std::cout << GREEN << "SUCCESS\n" << RESET;
	}

	{
		std::cout << "====== JoinErrorCheck METHOD ======\n";
		this->token_list_.clear();
		this->token_list_.push_back("JOIN");
		JoinCommand	com(this->token_list_, this->dummy_server_, this->dummy_client_);
		Channel dummy_channel("#dummy");
		channel_info info;
		memset(&info, 0, sizeof(info));
		assert(false == com.JoinErrorCheck(info));
		info.ch_ptr = &dummy_channel;
		assert(true == com.JoinErrorCheck(info));
		info.is_member = true;
		assert(false == com.JoinErrorCheck(info));
		info.is_member = false;
		info.mode = MODE_INVITE;
		assert(false == com.JoinErrorCheck(info));
		dummy_channel.Invite(this->dummy_client_->get_sock());
		assert(true == com.JoinErrorCheck(info));
		info.mode = MODE_KEY;
		assert(false == com.JoinErrorCheck(info));
		info.is_auth = true;
		assert(true == com.JoinErrorCheck(info));
		info.is_banned = true;
		assert(false == com.JoinErrorCheck(info));
		info.is_banned = false;
		info.mode = MODE_INVITE | MODE_KEY | MODE_TOPIC;
		info.is_auth = true;
		assert(true == com.JoinErrorCheck(info));
		info.is_auth = false;
		assert(false == com.JoinErrorCheck(info));
		std::cout << GREEN << "SUCCESS\n" << RESET;
	}
}

void	TestJoinCommand::RunTest(void) {
	//1-1) create channel
	this->dummy_client_->SetAuthFlag(AUTH);
	AddClient(this->dummy_client_);
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("#TEST");
	JoinCommand	com(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	exp = ":wooseoki!test@test JOIN #TEST\r\n353 wooseoki = #TEST :@wooseoki \r\n366 wooseoki #TEST\r\n";
	IsEqual(exp, RunAndReturnRespInTest(&com));

	//1-2.0 defalut mode
	Client	dc2(100);
	dc2.SetAuthFlag(AUTH);
	AddClient(&dc2);
	JoinCommand	com2(this->token_list_, this->dummy_server_, &dc2);
	std::string	exp2 = ":unknown!user@host JOIN #TEST\r\n353 unknown = #TEST :@wooseoki unknown \r\n366 unknown #TEST\r\n";
	IsEqual(exp2, RunAndReturnRespInTest(&com2));

	DeleteClient(dc2.get_sock());
	DeleteClient(this->dummy_client_->get_sock());
	this->dummy_client_->UnsetAuthFlagInTest();
	this->token_list_.clear();
	CeaseChannel("#TEST");

	//1-2.1 +i
	Client	dc3(101);
	dc3.set_nick("dc3");
	dc3.SetAuthFlag(AUTH);
	Channel	ch3("#TEST");
	ch3.set_mode(MODE_INVITE, true);
	ch3.Invite(dc3.get_sock());
	this->dummy_client_->SetAuthFlag(AUTH);
	AddClient(this->dummy_client_);
	AddClient(&dc3);
	AddChannel(&ch3);
	
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("#TEST");
	JoinCommand	com3(this->token_list_, this->dummy_server_, this->dummy_client_);
	JoinCommand	com3_1(this->token_list_, this->dummy_server_, &dc3);
	
	std::string	fail3 = "473 wooseoki #TEST :Cannot join channel (+i)";
	std::string	succ3 = ":dc3!user@host JOIN #TEST\r\n353 dc3 = #TEST :dc3 \r\n366 dc3 #TEST\r\n";
	IsEqual(fail3, RunAndReturnRespInTest(&com3));
	IsEqual(succ3, RunAndReturnRespInTest(&com3_1));
	this->token_list_.clear();
	DeleteClient(this->dummy_client_->get_sock());
	DeleteClient(dc3.get_sock());
	DeleteChannel(ch3.get_name());

	//1-2.2 +k
	Client	dc4(102);
	dc4.SetAuthFlag(AUTH);
	dc4.set_nick("dc4");

	Channel	ch4("#TEST-KEY");
	ch4.set_mode(MODE_KEY, true);
	ch4.set_key("succ_key");

	AddClient(&dc4);
	AddClient(this->dummy_client_);
	AddChannel(&ch4);

	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("#TEST-KEY");
	this->token_list_.push_back("fail_key");
	JoinCommand	com4_fail(this->token_list_, this->dummy_server_, this->dummy_client_);
	this->token_list_.pop_back();
	this->token_list_.push_back("succ_key");
	JoinCommand com4_succ(this->token_list_, this->dummy_server_, &dc4);
	std::string	fail4 = "475 wooseoki #TEST-KEY :Cannot join channel (+k)";
	std::string	succ4 = ":dc4!user@host JOIN #TEST-KEY\r\n353 dc4 = #TEST-KEY :dc4 \r\n366 dc4 #TEST-KEY\r\n";
	IsEqual(fail4, RunAndReturnRespInTest(&com4_fail));
	IsEqual(succ4, RunAndReturnRespInTest(&com4_succ));

	this->token_list_.clear();
	DeleteClient(dc4.get_sock());
	DeleteClient(this->dummy_client_->get_sock());
	DeleteChannel(ch4.get_name());

	//1-2.3 +t
	Channel	ch5("&TEST-TOPIC");
	ch5.set_mode(MODE_TOPIC, true);
	ch5.set_topic("topic!");

	AddClient(this->dummy_client_);
	AddChannel(&ch5);

	this->token_list_.push_back("JOIN");
	this->token_list_.push_back(ch5.get_name());
	JoinCommand	com5(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	succ5 = ":wooseoki!test@test JOIN &TEST-TOPIC\r\n332 wooseoki &TEST-TOPIC :topic!\r\n";
	succ5 = succ5 + "353 wooseoki = &TEST-TOPIC :wooseoki \r\n366 wooseoki &TEST-TOPIC\r\n";
	IsEqual(succ5, RunAndReturnRespInTest(&com5));

	this->token_list_.clear();
	DeleteClient(this->dummy_client_->get_sock());
	DeleteChannel(ch5.get_name());

	// not registered client's join request
	this->dummy_client_->UnsetAuthFlagInTest();
	AddClient(this->dummy_client_);
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("#TEST2");
	JoinCommand	com6(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	fail6 = "400 wooseoki JOIN :Not registered in Server";
	IsEqual(fail6, RunAndReturnRespInTest(&com6));
	this->token_list_.clear();
	DeleteClient(this->dummy_client_->get_sock());

	// join failure caused by channel number limit
	Channel	ch7("&TEST-LIMIT");
	ch7.set_limit(1);
	ch7.Join(-1, ' ');
	this->dummy_client_->SetAuthFlag(AUTH);
	AddClient(this->dummy_client_);
	AddChannel(&ch7);
	token_list_.push_back("JOIN");
	token_list_.push_back("&TEST-LIMIT");
	JoinCommand	com7(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	fail7 = "471 wooseoki &TEST-LIMIT :Cannot join channel (+l)";
	IsEqual(fail7, RunAndReturnRespInTest(&com7));
	this->token_list_.clear();
	DeleteClient(this->dummy_client_->get_sock());
	DeleteChannel(ch7.get_name());
	this->dummy_client_->UnsetAuthFlagInTest();

	// join failure caused by already joined client's request
	Channel	ch8("&TEST-MEMBER");
	ch8.Join(this->dummy_client_->get_sock(), '@');
	this->dummy_client_->SetAuthFlag(AUTH);
	AddClient(this->dummy_client_);
	AddChannel(&ch8);
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("&TEST-MEMBER");
	JoinCommand	com8(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	fail8 = "400 :wooseoki is already in &TEST-MEMBER";
	IsEqual(fail8, RunAndReturnRespInTest(&com8));
	this->token_list_.clear();
	this->dummy_client_->UnsetAuthFlagInTest();
	DeleteClient(this->dummy_client_->get_sock());
	DeleteChannel(ch8.get_name());

	// join failure caused by bad channel mask
	this->dummy_client_->SetAuthFlag(AUTH);
	AddClient(this->dummy_client_);
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("BAD");
	JoinCommand	com9(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	fail9 = "476 BAD :Bad Channel Mask";
	IsEqual(fail9, RunAndReturnRespInTest(&com9));
	this->token_list_.clear();
	this->dummy_client_->UnsetAuthFlagInTest();
	DeleteClient(this->dummy_client_->get_sock());

	// join failure caused by bad channel name
	this->dummy_client_->SetAuthFlag(AUTH);
	AddClient(this->dummy_client_);
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("#");
	JoinCommand	com10(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	fail10 = "403 wooseoki # :No such channel";
	IsEqual(fail10, RunAndReturnRespInTest(&com10));
	this->token_list_.clear();
	this->dummy_client_->UnsetAuthFlagInTest();
	DeleteClient(this->dummy_client_->get_sock());

	// join fail with wrong key string
	this->dummy_client_->SetAuthFlag(AUTH);
	AddClient(this->dummy_client_);
	this->token_list_.push_back("JOIN");
	this->token_list_.push_back("#TEST");
	this->token_list_.push_back("key with whitespace");
	JoinCommand	com11(this->token_list_, this->dummy_server_, this->dummy_client_);
	std::string	fail11 = "400 :key with whitespace";
	IsEqual(fail11, RunAndReturnRespInTest(&com11));
	this->token_list_.clear();
	this->dummy_client_->UnsetAuthFlagInTest();
	DeleteClient(this->dummy_client_->get_sock());
}

void	TestJoinCommand::TearDown(void) {

}
