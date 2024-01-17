#include "Request.hpp"
#include "SpecifyCommand.hpp"

std::vector<Command*> Request::ParseRequest(Server *server, Client *client, std::string request, int *offset) {
	std::vector<std::string> message_list;
	std::vector<Command *> command_list;

	*offset = SplitRequest(request, &message_list);
	SplitMessage(server, client, message_list, &command_list);
	return command_list;
}

int	Request::SplitRequest(const std::string &request, std::vector<std::string> *message_list) {
	static const std::string	delimiter = "\r\n";
	size_t start = 0, end = 0;

	while ((end = request.find(delimiter, start)) != std::string::npos) {
		message_list->push_back(request.substr(start, end - start));
		start = end + delimiter.length();
	}
	// Need log file
	if (start != request.length())
		std::cerr << "Unvalid message format\n";
	return start;
}

void	Request::SplitMessage(Server *server, Client *client, const std::vector<std::string> &message_list, std::vector<Command *> *command_list) {
	std::vector<std::string>	token_list;
	std::string msg;
	Command	*c;

	for (size_t i = 0; i < message_list.size(); ++i) {
		if (message_list[i][0] == ' ')
			continue ;
		msg = RemoveDuplicateSpace(message_list[i]);
		token_list.clear();
		SeperateWhiteSpace(msg, &token_list);
		c = CommandFactory(token_list);
		if (c != NULL) {
			c->set_server(server);
			c->set_client(client);
			command_list->push_back(c);
		}
	}
}

// Message can be seperated one or more whitespace
std::string Request::RemoveDuplicateSpace(const std::string& str) {
	std::string result;
	bool isSpace = false;
	bool isColon = false;

	for (size_t i = 0; i < str.size(); ++i) {
		if (isColon == false && str[i] == ' ') {
			if (isSpace == false) {
				result += ' ';
				isSpace = true;
			}
		} else if (i != 0 && str[i] == ':') {
			isColon = true;
		} else {
			result += str[i];
			isSpace = false;
		}
	}
	return result;
}

// The colon not prefix is mean last parameter and doesn't need remove white spaces
void	Request::SeperateWhiteSpace(const std::string &str, std::vector<std::string> *token_list) {
	static const char delimiter = ' ';
	size_t	start = 0, end = 0;

	while ((end = str.find(delimiter, start)) != std::string::npos) {
		token_list->push_back(str.substr(start, end - start));
		start = end + 1;
		if (start < str.length() && str[start] == ':')
			break ;
	}
	token_list->push_back(str.substr(start));
}

int	Request::BaseAlphaToNumber(const std::string &token) {
	int		index = 0;
	size_t	acc = 0;

	while (token[index]) {
		if (token[index] >= 'a' && token[index] <= 'z') {
			acc *= 26;
			acc += token[index] - 'a' + 1;
		} else if (token[index] >= 'A' && token[index] <= 'Z') {
			acc *= 26;
			acc += token[index] - 'A' + 1;
		} else
			return 0;
		++index;
	}
	std::cout << "acc : " << acc << "\n";
	return acc % static_cast<size_t>(INTMAX);
}

int	Request::SearchCommand(const std::vector<std::string> &token_list) {
	int	acc = 0;

	// first token is <prefix> and second is <command>
	if (token_list[0][0] == ':' && token_list.size() > 1)
		acc = BaseAlphaToNumber(token_list[1]);
	else
		acc = BaseAlphaToNumber(token_list[0]);

	std::cout << "token : " << token_list[0] << "\n";
	return acc;
}

Command *	Request::CommandFactory(const std::vector<std::string> &token_list) {
	Command *c = NULL;

	switch (SearchCommand(token_list)) {
		case CAP:
			std::cout << "CAP IN\n";
			c = new CapCommand(token_list);
			break ;
		case JOIN:
			std::cout << "JOIN IN\n";
			//JoinCommand(token_list);
			break ;
		case KICK:
			std::cout << "KICK IN\n";
			//KickCommand(token_list);
			break ;
		case MODE:
			std::cout << "MODE IN\n";
			break ;
		case NICK:
			std::cout << "NICK IN\n";		
			break ;
		case PING:
			std::cout << "PING IN\n";
			break ;
		case PASS:
			std::cout << "PASS IN\n";
			break ;
		case PART:
			std::cout << "PARK IN\n";
			break ;
		case PONG:
			std::cout << "PONG IN\n";
			break ;
		case USER:
			std::cout << "USER IN\n";
			break ;
		case QUIT:
			std::cout << "QUIT IN\n";
			break ;
		case TOPIC:
			std::cout << "TOPIC IN\n";
			break ;
		case WHOIS:
			std::cout << "WHOIS IN\n";
			break ;
		case INVITE:
			std::cout << "INVITE IN\n";
			break ;
		case NOTICE:
			std::cout << "NOTICE IN\n";
			break ;
		case PRIVMSG:
			std::cout << "PRIVMSG IN\n";
			break ;
		default:
			std::cout << "Command not found : ";
	}
	return c;
}
