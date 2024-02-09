#include "TestResponse.hpp"

void	TestCapCommand(Server *s, Client *dc) {
	std::cout << "====== CAPCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("CAP");
	CapCommand com(token_list, s, dc);
	IsEqual("461 CAP :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.push_back("LS");
	CapCommand comm(token_list, s, dc);
	IsEqual("CAP * LS :", comm.RunAndReturnRespInTest());

	dc->UnsetAuthFlagInTest();
}

void	TestPingCommand(Server *s, Client *dc) {
	std::cout << "====== PINGCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("PING");
	PingCommand com(token_list, s, dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("461 PING :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("PING");
	token_list.push_back("localhost");
	PingCommand com2(token_list, s, dc);
	IsEqual("PONG localhost", com2.RunAndReturnRespInTest());

	dc->UnsetAuthFlagInTest();
}

void	TestNickCommand(Server *s, Client *dc) {
	std::cout << "====== NICKCOMMAND ======\n";
	std::vector<std::string> token_list;

	//1 : Parameter number error 431
	//1-1
	std::cout << "case1) NICK with no parameters [Non-Auth Client]\n";
	token_list.push_back("NICK");
	s->AddClient(dc);
	NickCommand	com(token_list, s, dc);
	IsEqual("431 :No nickname given", com.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());

	//1-2
	std::cout << "case2) NICK with no parameters [Auth Client]\n";
	dc->SetAuthFlag(FT_AUTH);
	s->AddClient(dc);
	IsEqual("431 :No nickname given", com.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());
	dc->UnsetAuthFlagInTest();
	token_list.clear();

	//2 : Duplicated nickname error 433
	Client	already_in_client;
	std::string	dup_name = "dup";
	already_in_client.set_nick(dup_name);
	already_in_client.set_sock(100);
	s->AddClient(&already_in_client);
	token_list.push_back("NICK");
	token_list.push_back(dup_name);
	NickCommand	com2(token_list, s, dc);

	//2-1
	std::cout << "case2) NICK not_unique_nick [Non-Auth Client]\n";
	s->AddClient(dc);
	IsEqual("433 dup :Nickname is already in use", com2.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());

	//2-2
	std::cout << "case3) NICK not_unique_nick [Auth Client]\n";
	dc->SetAuthFlag(FT_AUTH);
	s->AddClient(dc);
	IsEqual("433 dup :Nickname is already in use", com2.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());
	s->DeleteClient(already_in_client.get_sock());
	dc->UnsetAuthFlagInTest();

	//3 : Invalid param 432
	token_list.clear();
	token_list.push_back("NICK");
	token_list.push_back("#tt");
	NickCommand	com3(token_list, s, dc);

	//3-1
	std::cout << "case3) NICK invalid_nick [Non-Auth Client]\n";
	s->AddClient(dc);
	IsEqual("432 #tt :Erroneus nickname", com3.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());

	std::cout << "case3) NICK invalid_nick [Auth Client]\n";
	dc->SetAuthFlag(FT_AUTH);
	s->AddClient(dc);
	IsEqual("432 #tt :Erroneus nickname", com3.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());
	dc->UnsetAuthFlagInTest();

	//4 : Nick valid
	token_list.clear();
	token_list.push_back("NICK");
	token_list.push_back("nick");
	NickCommand com4(token_list, s, dc);
	s->AddClient(dc);

	//4-1
	std::cout << "case4) NICK valid_nick [Non-Auth Client]\n";
	IsEqual("", com4.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());

	dc->SetAuthFlag(FT_AUTH);
	s->AddClient(dc);
	token_list.clear();
	token_list.push_back("NICK");
	token_list.push_back("new-nick");
	NickCommand com4_1(token_list, s, dc);

	std::cout << "case4) NICK valid_nick [Auth Client]\n";
	IsEqual(":nick NICK new-nick", com4_1.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back(":nick");
	token_list.push_back("NICK");
	token_list.push_back("new-nick2");
	NickCommand com4_2(token_list, s, dc);
	std::cout << "case4) :prev_nick NICK valid_nick [Auth Client]\n";
	IsEqual(":nick NICK new-nick2", com4_2.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());
}

void	TestPassCommand(Server *s, Client *dc) {
	std::cout << "====== PASSCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("PASS");
	PassCommand	com_no_param(token_list, s, dc);
	dc->UnsetAuthFlagInTest();
	s->AddClient(dc);
	IsEqual("400 :parameter number error", com_no_param.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());

	dc->SetAuthFlag(FT_AUTH);
	s->AddClient(dc);
	IsEqual("451 PASS :already registered", com_no_param.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());

	dc->UnsetAuthFlagInTest();
	s->AddClient(dc);
	token_list.push_back("wrong-password");
	PassCommand	com_wrong(token_list, s, dc);
	IsEqual("464 :Password incorrect", com_wrong.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());
	dc->UnsetAuthFlagInTest();
}

void	TestUserCommand(Server *s, Client *dc) {
	std::cout << "====== USERCOMMAND ======\n";
	std::vector<std::string> token_list;
	s->AddClient(dc);
	
	token_list.push_back("USER");
	UserCommand	com_no_param(token_list, s, dc);
	IsEqual("461 USER :Not enough parameters", com_no_param.RunAndReturnRespInTest());

	token_list.push_back("white space");
	UserCommand	com_less_param(token_list, s, dc);
	IsEqual("461 USER :Not enough parameters", com_less_param.RunAndReturnRespInTest());

	token_list.push_back("hostname");
	UserCommand	com_less_param2(token_list, s, dc);
	IsEqual("461 USER :Not enough parameters", com_less_param2.RunAndReturnRespInTest());

	token_list.push_back("servername");
	UserCommand	com_less_param3(token_list, s, dc);
	IsEqual("461 USER :Not enough parameters", com_less_param3.RunAndReturnRespInTest());

	token_list.push_back("real name");
	UserCommand	com_white_space(token_list, s, dc);
	IsEqual("400 :username must not has whitespace", \
			com_white_space.RunAndReturnRespInTest());

	s->DeleteClient(dc->get_sock());
	token_list.clear();
	dc->SetAuthFlag(FT_AUTH);
	token_list.push_back("USER");
	token_list.push_back("test");
	token_list.push_back("test");
	token_list.push_back("test");
	token_list.push_back("test test test");
	s->AddClient(dc);

	UserCommand	com(token_list, s, dc);
	IsEqual("462 USER :already registered", com.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());

	dc->UnsetAuthFlagInTest();
	s->AddClient(dc);
	UserCommand	com2(token_list, s, dc);
	IsEqual("", com2.RunAndReturnRespInTest());
	s->DeleteClient(dc->get_sock());
}

void	TestJoinCommand(Server *s, Client *dc) {
	std::cout << "====== JOINCOMMAND ======\n";
	std::vector<std::string> token_list;
	token_list.push_back("JOIN");
}

void	TestPartCommand(Server *s, Client* dc) {
	std::cout << "====== PARTCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("PART");
	s->AddClientInTest(dc->get_sock(), *dc);
	PartCommand com(token_list, s, dc);
	com.set_server(s);
	com.set_client(dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("461 PART :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("PART");
	token_list.push_back("#empty");
	PartCommand com2(token_list, s, dc);
	IsEqual("403 #empty :No such channel", com2.RunAndReturnRespInTest());

	s->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	s->AddChannelInTest("#dummy", dummy_channel);
	token_list.clear();
	token_list.push_back("PART");
	token_list.push_back("#dummy");
	PartCommand com3(token_list, s, dc);
	IsEqual("442 #dummy :You're not on that channel", com3.RunAndReturnRespInTest());

	PartCommand com4(token_list, s, dc);
	Channel *ch_ptr;
	ch_ptr = s->get_channel_ptr("#dummy");
	ch_ptr->Join(dc->get_sock(), '@');
	IsEqual(":wooseoki PART #dummy", com4.RunAndReturnRespInTest());

	s->DeleteClientInTest(dc->get_sock());
	s->DeleteChannelMutex("#dummy");
	s->DeleteChannelInTest("#dummy");
	dc->UnsetAuthFlagInTest();
}

void	TestWhoisCommand(Server *s, Client *dc) {
	std::cout << "====== WHOISCOMMAND ======\n";
	std::vector<std::string> token_list;
	
	token_list.push_back("WHOIS");
	WhoisCommand com(token_list, s, dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("431 :No nickname given", com.RunAndReturnRespInTest());

	token_list.push_back("dummy_client");
	WhoisCommand com2(token_list, s, dc);
	IsEqual("401 dummy_client :No such nick", com2.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("WHOIS");
	token_list.push_back(dc->get_nick());
	s->AddClientInTest(dc->get_sock(), *dc);
	WhoisCommand com3(token_list, s, dc);
	IsEqual("318 WHOISEND", com3.RunAndReturnRespInTest());

	s->DeleteClientInTest(dc->get_sock());
	dc->UnsetAuthFlagInTest();
}

void	TestQuitCommand(Server *s, Client *dc) {
	std::cout << "====== QUITCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("QUIT");
	s->AddClientInTest(dc->get_sock(), *dc);
	QuitCommand com(token_list, s, dc);
	IsEqual(":wooseoki QUIT :Quit: Bye for now!", com.RunAndReturnRespInTest());

	s->DeleteClientInTest(dc->get_sock());
}

void	TestTopicCommand(Server *s, Client *dc) {
	std::cout << "====== TOPICCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("TOPIC");
	TopicCommand com(token_list, s, dc);
	s->AddClientInTest(dc->get_sock(), *dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("461 TOPIC :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("TOPIC");
	token_list.push_back("#empty");
	TopicCommand com2(token_list, s, dc);
	IsEqual("331 :No topic is set", com2.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("TOPIC");
	token_list.push_back("#empty");
	token_list.push_back("dummy_topic");
	TopicCommand com5(token_list, s, dc);
	IsEqual("403 :No such channel", com5.RunAndReturnRespInTest());

	s->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	s->AddChannelInTest("#dummy", dummy_channel);
	token_list.clear();
	token_list.push_back("TOPIC");
	token_list.push_back("#dummy");
	token_list.push_back("dummy_topic");
	TopicCommand com3(token_list, s, dc);
	IsEqual("442 #dummy :You're not on that channel", com3.RunAndReturnRespInTest());

	TopicCommand com4(token_list, s, dc);
	Channel *ch_ptr;
	ch_ptr = s->get_channel_ptr("#dummy");
	ch_ptr->Join(dc->get_sock(), ' ');
	ch_ptr->set_mode(MODE_TOPIC, true);
	IsEqual("482 #dummy :You're not channel operator", com4.RunAndReturnRespInTest());
	ch_ptr->set_mode(MODE_TOPIC, false);
	IsEqual(":wooseoki TOPIC #dummy :dummy_topic", com4.RunAndReturnRespInTest());

	s->DeleteClientInTest(dc->get_sock());
	s->DeleteChannelMutex("#dummy");
	s->DeleteChannelInTest("#dummy");
	dc->UnsetAuthFlagInTest();
}

void	TestPrivmsgCommand(Server *s, Client *dc) {
	std::cout << "====== PRIVMSGCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("PRIVMSG");
	s->AddClientInTest(dc->get_sock(), *dc);
	PrivmsgCommand com(token_list, s, dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("412 :No text to send", com.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("PRIVMSG");
	token_list.push_back("#empty");
	PrivmsgCommand com2(token_list, s, dc);
	IsEqual("403 :No such channel", com2.RunAndReturnRespInTest());

	s->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	s->AddChannelInTest("#dummy", dummy_channel);
	token_list.clear();
	token_list.push_back("PRIVMSG");
	token_list.push_back("#dummy");
	token_list.push_back("dummy_msg");
	PrivmsgCommand com3(token_list, s, dc);
	IsEqual("404 #dummy :Can not send to chanel", com3.RunAndReturnRespInTest());

	PrivmsgCommand com4(token_list, s, dc);
	Channel *ch_ptr;
	ch_ptr = s->get_channel_ptr("#dummy");
	ch_ptr->Join(dc->get_sock(), ' ');
	IsEqual("411 :No recepient given", com4.RunAndReturnRespInTest());
	Client dummy_client;
	dummy_client.set_sock(8);
	dummy_client.set_nick("saseo");
	ch_ptr->Join(dummy_client.get_sock(), ' ');
	IsEqual(":wooseoki PRIVMSG #dummy :dummy_msg", com4.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("PRIVMSG");
	token_list.push_back("saseo");
	token_list.push_back("dummy_msg");
	PrivmsgCommand com5(token_list, s, dc);
	s->AddClientInTest(dummy_client.get_sock(), dummy_client);
	IsEqual(":wooseoki PRIVMSG saseo :dummy_msg", com5.RunAndReturnRespInTest());

	s->DeleteClientInTest(dc->get_sock());
	s->DeleteClientInTest(dummy_client.get_sock());
	s->DeleteChannelMutex("#dummy");
	s->DeleteChannelInTest("#dummy");
	dc->UnsetAuthFlagInTest();
}

void	TestKickCommand(Server *s, Client *dc) {
	std::cout << "====== KICKCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("KICK");
	s->AddClientInTest(dc->get_sock(), *dc);
	KickCommand com(token_list, s, dc);
	IsEqual("461 KICK :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.push_back("#dummy");
	token_list.push_back("saseo");
	KickCommand com2(token_list, s, dc);
	IsEqual("401 saseo :No such nick", com2.RunAndReturnRespInTest());

	KickCommand com3(token_list, s, dc);
	Client dummy_client;
	dummy_client.set_sock(8);
	dummy_client.set_nick("saseo");
	s->AddClientInTest(dummy_client.get_sock(), dummy_client);
	IsEqual("403 #dummy :No such channel", com3.RunAndReturnRespInTest());

	s->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	s->AddChannelInTest("#dummy", dummy_channel);
	IsEqual("442 #dummy :You're not on that channel", com3.RunAndReturnRespInTest());

	Channel *ch_ptr;
	ch_ptr = s->get_channel_ptr("#dummy");
	ch_ptr->Join(dc->get_sock(), ' ');
	IsEqual("441 saseo #dummy :They aren't on the channel", com3.RunAndReturnRespInTest());

	ch_ptr->Join(dummy_client.get_sock(), ' ');
	IsEqual("482 #dummy :You're not channel operator", com3.RunAndReturnRespInTest());

	ch_ptr->Mode(dc->get_sock(), '@');
	IsEqual(":wooseoki KICK #dummy saseo", com3.RunAndReturnRespInTest());

	s->DeleteClientInTest(dc->get_sock());
	s->DeleteClientInTest(dummy_client.get_sock());
	s->DeleteChannelMutex("#dummy");
	s->DeleteChannelInTest("#dummy");
	dc->UnsetAuthFlagInTest();
}

void	TestModeCommand(Server *s, Client *dc) {
	std::cout << "====== ModeCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("MODE");
	s->AddClientInTest(dc->get_sock(), *dc);
	ModeCommand com(token_list, s, dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("461 MODE :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.push_back("dummy_param");
	ModeCommand com2(token_list, s, dc);
	IsEqual("324 :Not given modestring", com2.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("MODE");
	token_list.push_back("#dummy");
	token_list.push_back("+unvalidmodestr");
	ModeCommand com3(token_list, s, dc);
	IsEqual("472 +unvalidmodestr :is unknown mode char to me", com3.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("MODE");
	token_list.push_back("#dummy");
	token_list.push_back("+kl-o");
	token_list.push_back("dummy_param");
	ModeCommand com4(token_list, s, dc);
	IsEqual("461 :No match param count", com4.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("MODE");
	token_list.push_back("#dummy");
	token_list.push_back("+t");
	ModeCommand com5(token_list, s, dc);
	IsEqual("403 #dummy :No such channel", com5.RunAndReturnRespInTest());

	s->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	s->AddChannelInTest("#dummy", dummy_channel);
	IsEqual("442 #dummy :You're not on that channel", com5.RunAndReturnRespInTest());

	Channel *ch_ptr;
	ch_ptr = s->get_channel_ptr("#dummy");
	ch_ptr->Join(dc->get_sock(), ' ');
	IsEqual("482 #dummy :You're not channel operator", com5.RunAndReturnRespInTest());

	ch_ptr->Mode(dc->get_sock(), '@');
	IsEqual(":wooseoki MODE #dummy +t", com5.RunAndReturnRespInTest());

	s->DeleteClientInTest(dc->get_sock());
	s->DeleteChannelMutex("#dummy");
	s->DeleteChannelInTest("#dummy");
	dc->UnsetAuthFlagInTest();
}

void	TestInviteCommand(Server *s, Client *dc) {
	std::cout << "====== INVITECOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("INVITE");
	s->AddClientInTest(dc->get_sock(), *dc);
	InviteCommand com(token_list, s, dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("461 INVITE :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.push_back("saseo");
	token_list.push_back("#dummy");
	InviteCommand com2(token_list, s, dc);
	IsEqual("401 saseo :No such nick", com2.RunAndReturnRespInTest());
	Client dummy_client;
	dummy_client.set_sock(8);
	dummy_client.set_nick("saseo");
	s->AddClientInTest(dummy_client.get_sock(), dummy_client);
	IsEqual("403 #dummy :No such channel", com2.RunAndReturnRespInTest());

	s->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	s->AddChannelInTest("#dummy", dummy_channel);
	IsEqual("442 #dummy :You're not on that channel", com2.RunAndReturnRespInTest());

	Channel *ch_ptr;
	ch_ptr = s->get_channel_ptr("#dummy");
	ch_ptr->Join(dc->get_sock(), ' ');
	IsEqual("482 #dummy :You're not channel operator", com2.RunAndReturnRespInTest());
	ch_ptr->Mode(dc->get_sock(), '@');
	ch_ptr->Join(dummy_client.get_sock(), ' ');
	IsEqual("443 saseo #dummy :is already on cahnnel", com2.RunAndReturnRespInTest());
	ch_ptr->Kick(dummy_client.get_sock());
	IsEqual(":wooseoki 341 saseo #dummy", com2.RunAndReturnRespInTest());

	s->DeleteClientInTest(dc->get_sock());
	s->DeleteClientInTest(dummy_client.get_sock());
	s->DeleteChannelMutex("#dummy");
	s->DeleteChannelInTest("#dummy");
	dc->UnsetAuthFlagInTest();
}

void	TestUnvalidCommand(Server *s, Client *dc) {
	std::cout << "====== UNVALIDCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("blahblah");
	UnvalidCommand com(token_list, s, dc);
	IsEqual("421 blahblah :Unknown command", com.RunAndReturnRespInTest());
}

void	TestResponse(Server *s, Client *c) {
	std::cout << YELLOW << "RESPONSE TEST START\n" << RESET;
	TestPingCommand(s, c);
	TestPartCommand(s, c);
	TestCapCommand(s, c);
	TestWhoisCommand(s, c);
	TestQuitCommand(s, c);
	TestTopicCommand(s, c);
	TestPrivmsgCommand(s, c);
	TestKickCommand(s, c);
	TestModeCommand(s, c);
	TestInviteCommand(s, c);
	TestUnvalidCommand(s, c);
	TestNickCommand(s, c);
	TestPassCommand(s, c);
	TestUserCommand(s, c);
	std::cout << RED << "TEST FAILED: " << fail_count << RESET;
	std::cout << GREEN << " TEST SUCCESED: " << success_count << "\n" << RESET;
}
