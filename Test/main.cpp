#include "TestRequest.hpp"
#include "TestResponse.hpp"

Server	*ServerInit(void) {
	char *argv[] = {TEST_SERVER_NAME, TEST_SERVER_PORT, TEST_SERVER_PASSWORD};
	Server *result;

	result = new Server(3, argv);
	return result;
}

Client*	ClientInit(void) {
	Client *result;

	result = new Client(TEST_CLIENT_SOCK);
	result->set_nick(TEST_CLIENT_NICK);
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
