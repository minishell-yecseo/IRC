#include "temp.hpp"

void	IsStringEqual(const std::string& a, const std::string& b) {
	if (a.compare(b) != 0) {
		std::cout << RED << a << " != " << b << "\n" << RESET;
		return ;
	}
	std::cout << GREEN << a << " == " << b << "\n" << RESET;
}

void	IsIntEqual(int a, int b) {
	if (a != b) {
		std::cerr << RED << a << " != " << b << "\n" << RESET;
		return ;
	}
	std::cout << GREEN << a << " == " << b << "\n" << RESET;
}

void	IsVectorEqual(const std::vector<std::string> a, const std::vector<std::string>& b) {
	if (a.size() != b.size()) {
		std::cerr << RED << "size not same\n" << RESET;
	}

	for (size_t i = 0; i < a.size(); ++i) {
		if (a[i].compare(b[i]) != 0)
			std::cout << RED << a[i] << " != " << b[i] << "\n" << RESET;
		else
			std::cerr << GREEN << a[i] << " == " << b[i] << "\n" << RESET;
	}
}

void	TestSplitRequest(void) {
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

void	TestRemoveDuplicateSpace(void) {
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

void	TestRequest(Server *s, Client *c) {
	TestSplitRequest();
	TestRemoveDuplicateSpace();

}
