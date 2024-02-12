#include "TestRequest.hpp"
#include "TestResponse.hpp"

Server	*ServerInit(void) {
	char *argv[] = {"TEST", "9090", "1234"};
	Server *result;

	result = new Server(3, argv);
	return result;
}

Client*	ClientInit(void) {
	Client *result;

	result = new Client;
	result->set_sock(9);
	result->set_nick("wooseoki");
	return result;
}

Mutex print;

int main() {
	Server *s;
	Client *c;

	print.init(NULL);
	s = ServerInit();
	c = ClientInit();
	TestRequest(s, c);
	TestResponse(s, c);

	delete s;
	delete c;
}