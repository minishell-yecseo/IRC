#include <cassert>
#include <cstring>

#include "Request.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "log.hpp"
#include "Mutex.hpp"
#include "Utils.hpp"
#include "ThreadPool.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include "PingCommand.hpp"
#include "PartCommand.hpp"
#include "CapCommand.hpp"
#include "WhoisCommand.hpp"
#include "QuitCommand.hpp"
#include "TopicCommand.hpp"
#include "PrivmsgCommand.hpp"
#include "KickCommand.hpp"
#include "ModeCommand.hpp"
#include "InviteCommand.hpp"
#include "UnvalidCommand.hpp"

Mutex print;
size_t	fail_count = 0;
size_t	success_count = 0;

void	MakeToken(std::vector<std::string>* token_list) {
	
}

void	IsEmpty(const std::string& a) {
	if (a.empty() == false) {
		std::cerr << a << " :is not empty\n";
	}
}


void	IsEqual(std::string a, const std::string& b) {
	if (a.compare(b) != 0) {
		std::cerr << RED << a << " != " << b << "\n" << RESET;
		fail_count += 1;
		return ;
		// assert(false);
	}
	std::cout << a << " == " << b << "\n";
	success_count += 1;
}

void	TestCapCommand(Server *s, Client *dc) {
	std::cout << "====== CAPCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("CAP");
	CapCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	IsEqual("461 CAP :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.push_back("LS");
	CapCommand comm(token_list);
	comm.set_server(s);
	comm.set_client(dc);
	IsEqual("CAP * LS :", comm.RunAndReturnRespInTest());

	dc->UnsetAuthFlagInTest();
}

void	TestPingCommand(Server *s, Client *dc) {
	std::cout << "====== PINGCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("PING");
	PingCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("461 PING :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("PING");
	token_list.push_back("localhost");
	PingCommand com2(token_list);
	com2.set_server(s);
	com2.set_client(dc);
	IsEqual("PONG localhost", com2.RunAndReturnRespInTest());

	dc->UnsetAuthFlagInTest();
}

void	TestPartCommand(Server *s, Client* dc) {
	std::cout << "====== PARTCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("PART");
	s->AddClientInTest(dc->get_sock(), *dc);
	PartCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("461 PART :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("PART");
	token_list.push_back("#empty");
	PartCommand com2(token_list);
	com2.set_server(s);
	com2.set_client(dc);
	IsEqual("403 #empty :No such channel", com2.RunAndReturnRespInTest());

	s->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	s->AddChannelInTest("#dummy", dummy_channel);
	token_list.clear();
	token_list.push_back("PART");
	token_list.push_back("#dummy");
	PartCommand com3(token_list);
	com3.set_server(s);
	com3.set_client(dc);
	IsEqual("442 #dummy :You're not on that channel", com3.RunAndReturnRespInTest());

	PartCommand com4(token_list);
	com4.set_server(s);
	com4.set_client(dc);
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
	WhoisCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("431 :No nickname given", com.RunAndReturnRespInTest());

	token_list.push_back("dummy_client");
	WhoisCommand com2(token_list);
	com2.set_server(s);
	com2.set_client(dc);
	IsEqual("401 dummy_client :No such nick", com2.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("WHOIS");
	token_list.push_back(dc->get_nick());
	s->AddClientInTest(dc->get_sock(), *dc);
	WhoisCommand com3(token_list);
	com3.set_server(s);
	com3.set_client(dc);
	IsEqual("318 WHOISEND", com3.RunAndReturnRespInTest());

	s->DeleteClientInTest(dc->get_sock());
	dc->UnsetAuthFlagInTest();
}

void	TestQuitCommand(Server *s, Client *dc) {
	std::cout << "====== QUITCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("QUIT");
	s->AddClientInTest(dc->get_sock(), *dc);
	QuitCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	IsEqual(":wooseoki QUIT :Quit: Bye for now!", com.RunAndReturnRespInTest());

	s->DeleteClientInTest(dc->get_sock());
}

void	TestTopicCommand(Server *s, Client *dc) {
	std::cout << "====== TOPICCOMMAND ======\n";
	std::vector<std::string> token_list;

	token_list.push_back("TOPIC");
	TopicCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	s->AddClientInTest(dc->get_sock(), *dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("461 TOPIC :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("TOPIC");
	token_list.push_back("#empty");
	TopicCommand com2(token_list);
	com2.set_server(s);
	com2.set_client(dc);
	IsEqual("331 :No topic is set", com2.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("TOPIC");
	token_list.push_back("#empty");
	token_list.push_back("dummy_topic");
	TopicCommand com5(token_list);
	com5.set_server(s);
	com5.set_client(dc);
	IsEqual("403 :No such channel", com5.RunAndReturnRespInTest());

	s->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	s->AddChannelInTest("#dummy", dummy_channel);
	token_list.clear();
	token_list.push_back("TOPIC");
	token_list.push_back("#dummy");
	token_list.push_back("dummy_topic");
	TopicCommand com3(token_list);
	com3.set_server(s);
	com3.set_client(dc);
	IsEqual("442 #dummy :You're not on that channel", com3.RunAndReturnRespInTest());

	TopicCommand com4(token_list);
	com4.set_server(s);
	com4.set_client(dc);
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
	PrivmsgCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("412 :No text to send", com.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("PRIVMSG");
	token_list.push_back("#empty");
	PrivmsgCommand com2(token_list);
	com2.set_server(s);
	com2.set_client(dc);
	IsEqual("403 :No such channel", com2.RunAndReturnRespInTest());

	s->AddChannelMutex("#dummy");
	Channel dummy_channel("#dummy");
	s->AddChannelInTest("#dummy", dummy_channel);
	token_list.clear();
	token_list.push_back("PRIVMSG");
	token_list.push_back("#dummy");
	token_list.push_back("dummy_msg");
	PrivmsgCommand com3(token_list);
	com3.set_server(s);
	com3.set_client(dc);
	IsEqual("404 #dummy :Can not send to chanel", com3.RunAndReturnRespInTest());

	PrivmsgCommand com4(token_list);
	com4.set_server(s);
	com4.set_client(dc);
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
	PrivmsgCommand com5(token_list);
	com5.set_server(s);
	com5.set_client(dc);
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
	KickCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	IsEqual("461 KICK :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.push_back("#dummy");
	token_list.push_back("saseo");
	KickCommand com2(token_list);
	com2.set_server(s);
	com2.set_client(dc);
	IsEqual("401 saseo :No such nick", com2.RunAndReturnRespInTest());

	KickCommand com3(token_list);
	com3.set_server(s);
	com3.set_client(dc);
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
	ModeCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("461 MODE :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.push_back("dummy_param");
	ModeCommand com2(token_list);
	com2.set_server(s);
	com2.set_client(dc);
	IsEqual("324 :Not given modestring", com2.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("MODE");
	token_list.push_back("#dummy");
	token_list.push_back("+unvalidmodestr");
	ModeCommand com3(token_list);
	com3.set_server(s);
	com3.set_client(dc);
	IsEqual("472 +unvalidmodestr :is unknown mode char to me", com3.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("MODE");
	token_list.push_back("#dummy");
	token_list.push_back("+kl-o");
	token_list.push_back("dummy_param");
	ModeCommand com4(token_list);
	com4.set_server(s);
	com4.set_client(dc);
	IsEqual("461 :No match param count", com4.RunAndReturnRespInTest());

	token_list.clear();
	token_list.push_back("MODE");
	token_list.push_back("#dummy");
	token_list.push_back("+t");
	ModeCommand com5(token_list);
	com5.set_server(s);
	com5.set_client(dc);
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
	InviteCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	IsEqual("451 :You have not registered", com.RunAndReturnRespInTest());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("461 INVITE :Not enough parameters", com.RunAndReturnRespInTest());

	token_list.push_back("saseo");
	token_list.push_back("#dummy");
	InviteCommand com2(token_list);
	com2.set_server(s);
	com2.set_client(dc);
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
	UnvalidCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	IsEqual("421 blahblah :Unknown command", com.RunAndReturnRespInTest());
}

/*
void	TestParseRequest(Server *s, Client *c) {
	int	dummy;
	std::vector<Command *> dummy_list;

	Request::SplitMessage(s, c, "", &dummy_list)
	Request::ParseRequest(s, c, "TOPIC", &dummy);
}
*/

int main() {

	// for constructor error
	print.init(NULL);
	
	
	char	*argv[] = {"TEST", "9090", "1234"};
	Server s(3, argv);
	Client c;
	int	offset;
	//Request::ParseRequest(&s, &c, "a", &offset);
	c.set_sock(9);
	c.set_nick("wooseoki");

	TestPingCommand(&s, &c);
	TestPartCommand(&s, &c);
	TestCapCommand(&s, &c);
	TestWhoisCommand(&s, &c);
	TestQuitCommand(&s, &c);
	TestTopicCommand(&s, &c);
	TestPrivmsgCommand(&s, &c);
	TestKickCommand(&s, &c);
	TestModeCommand(&s, &c);
	TestInviteCommand(&s, &c);
	TestUnvalidCommand(&s, &c);
	std::cout << RED << "TEST FAILED: " << fail_count << RESET;
	std::cout << GREEN << " TEST SUCCESED: " << success_count << "\n" << RESET;
}
