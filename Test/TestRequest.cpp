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

Mutex print;

void	MakeToken(std::vector<std::string>* token_list) {
	
}

void	IsEmpty(const char *a) {
	if (strcmp(a, "") != 0) {
		std::cerr << a << " :is not empty\n";
	}
}

void	IsEqual(const char *a, const char *b) {
	if (strcmp(a, b) != 0) {
		std::cerr << a << " != " << b << "\n";
		assert(false);
	}
	std::cout << a << " == " << b << "\n";
}

void	TestCapCommand(Server *s, Client *dc) {
	std::cout << "====== CAPCOMMAND ======\n";
	std::vector<std::string> token_list;
	token_list.push_back("CAP");

	CapCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	
	IsEqual("461 CAP :Not enough parameters", com.AnyOfError().c_str());

	token_list.push_back("LS");
	CapCommand comm(token_list);
	com.set_server(s);
	com.set_client(dc);

	IsEmpty(comm.AnyOfError().c_str());
	s->DeleteClientInTest(dc->get_sock());
	s->DeleteChannelInTest("#dummy");
	dc->UnsetAuthFlagInTest();
}

void	TestPingCommand(Server *s, Client *dc) {
	std::cout << "====== PINGCOMMAND ======\n";
	std::vector<std::string> token_list;
	token_list.push_back("PING");

	PingCommand com(token_list);
	com.set_server(s);
	com.set_client(dc);
	
	IsEqual("451 :You have not registered", com.AnyOfError().c_str());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("461 PING :Not enough parameters", com.AnyOfError().c_str());
	s->DeleteClientInTest(dc->get_sock());
	s->DeleteChannelInTest("#dummy");
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
	
	IsEqual("451 :You have not registered", com.AnyOfError().c_str());
	dc->SetAuthFlag(FT_AUTH);
	IsEqual("461 PART :Not enough parameters", com.AnyOfError().c_str());

	token_list.clear();
	token_list.push_back("PART");
	PartCommand comm(token_list);

	comm.set_server(s);
	comm.set_client(dc);
	dc->SetAuthFlag(FT_AUTH);

	IsEqual("403 #empty :No such channel", comm.CheckChannel("#empty").c_str());
	Channel dummy_channel("#dummy");
	s->AddChannelMutex("#dummy");
	s->AddChannelInTest("#dummy", dummy_channel);
	IsEqual("442 #dummy :You're not on that channel", comm.CheckChannel("#dummy").c_str());
	Channel *ch_ptr;
	ch_ptr = s->get_channel_ptr("#dummy");
	ch_ptr->Join(dc->get_sock());
	IsEqual(":wooseoki PART #dummy", comm.CheckChannel("#dummy").c_str());
	s->DeleteClientInTest(dc->get_sock());
	s->DeleteChannelInTest("#dummy");
	dc->UnsetAuthFlagInTest();
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
}
