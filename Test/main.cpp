#include "TestRequest.hpp"
#include "TestResponse.hpp"
#include "TestChannel.hpp"

Server	*ServerInit(void) {
	Server *result;
	char	*name = strdup(TEST_SERVER_NAME);
	char	*port = strdup(TEST_SERVER_PORT);
	char	*pass = strdup(TEST_SERVER_PASSWORD);
	char 	*argv[3] = {name, port, pass};

	result = new Server(3, argv);
	free(name);
	free(port);
	free(pass);
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
	TestChannel();
	TestRequest();
	TestResponse(s, c);

	delete s;
	delete c;

	if (fail_count > 0)
		return 1;
	return 0;
}
