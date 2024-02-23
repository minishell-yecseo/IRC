#include "TestRequest.hpp"

TestRequest::TestRequest(void) {
	TestSplitRequest();
	TestRemoveDuplicateSpace();
	TestSeperateWhiteSpace();
	TestBaseAlphaToNumber();
}

void	TestRequest::TestSplitRequest(void) {
	int offset;
	std::string	request;
	std::vector<std::string> *mock = new std::vector<std::string>();
	std::vector<std::string> *result;

	request = "command\r\n";
	mock->push_back("command");
	result = Request::SplitRequest(request, &offset);
	IsVectorEqual(*mock, *result);
	mock->clear();
	delete result;

	request = "\r\ncommand\r\n";
	mock->push_back("");
	mock->push_back("command");
	result = Request::SplitRequest(request, &offset);
	IsVectorEqual(*mock, *result);
	mock->clear();
	delete result;

	request = "\n\r\rc\nom \r\tma\t\rn d\r\n";
	mock->push_back("\n\r\rc\nom \r\tma\t\rn d");
	result = Request::SplitRequest(request, &offset);
	IsVectorEqual(*mock, *result);
	mock->clear();
	delete result;

	request = "a\r\nb\r\nc\r\n";
	mock->push_back("a");
	mock->push_back("b");
	mock->push_back("c");
	result = Request::SplitRequest(request, &offset);
	IsVectorEqual(*mock, *result);
	mock->clear();
	delete result;

	request = ":prefix command\r\ncommand param\r\ncommand param param :last param\r\n";
	mock->push_back(":prefix command");
	mock->push_back("command param");
	mock->push_back("command param param :last param");
	result = Request::SplitRequest(request, &offset);
	IsVectorEqual(*mock, *result);
	mock->clear();
	delete result;

	request = "a\r\nb\r\nc\r\nasdf";
	mock->push_back("a");
	mock->push_back("b");
	mock->push_back("c");
	result = Request::SplitRequest(request, &offset);
	IsVectorEqual(*mock, *result);
	IsIntEqual(offset, request.length() - 4);
	mock->clear();
	delete result;

	delete mock;
}

void	TestRequest::TestRemoveDuplicateSpace(void) {
	std::string	message;
	std::string	result;
	std::string	mock;

	message = ":prefix       command";
	mock = ":prefix command";
	result = Request::RemoveDuplicateSpace(message);
	IsStringEqual(mock, result);

	message = "  :prefix       command";
	mock = ":prefix command";
	result = Request::RemoveDuplicateSpace(message);
	IsStringEqual(mock, result);

	message = ":prefix    command param :param   with   colon  ";
	mock = ":prefix command param :param   with   colon  ";
	result = Request::RemoveDuplicateSpace(message);
	IsStringEqual(mock, result);
}

void	TestRequest::TestSeperateWhiteSpace(void) {
	std::cout << "SEPREATEWHITESPACE\n";
	std::string	request;
	std::vector<std::string> mock;
	std::vector<std::string> result;

	request = "command";
	mock.push_back("command");
	Request::SeperateWhiteSpace(request, &result);
	IsVectorEqual(mock, result);
	mock.clear();
	result.clear();

	request = ":prefix command param";
	mock.push_back(":prefix");
	mock.push_back("command");
	mock.push_back("param");
	Request::SeperateWhiteSpace(request, &result);
	IsVectorEqual(mock, result);
	mock.clear();
	result.clear();

	request = ":prefix command param :last param with   multiple  space";
	mock.push_back(":prefix");
	mock.push_back("command");
	mock.push_back("param");
	mock.push_back("last param with   multiple  space");
	Request::SeperateWhiteSpace(request, &result);
	IsVectorEqual(mock, result);
	mock.clear();
	result.clear();
}

void	TestRequest::TestBaseAlphaToNumber(void) {
	int	result;
	std::string	request;

	request = "";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, 0);

	request = "CAP";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, CAP);

	request = "INVITE";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, INVITE);

	request = "JOIN";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, JOIN);

	request = "KICK";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, KICK);

	request = "MODE";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, MODE);

	request = "NICK";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, NICK);

	request = "PART";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, PART);

	request = "PASS";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, PASS);

	request = "PING";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, PING);

	request = "PRIVMSG";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, PRIVMSG);

	request = "QUIT";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, QUIT);

	request = "TOPIC";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, TOPIC);

	request = "USER";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, USER);

	request = "WHOIS";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, WHOIS);

	request = ":prefix";
	result = Request::BaseAlphaToNumber(request);
	IsIntEqual(result, 0);
}
