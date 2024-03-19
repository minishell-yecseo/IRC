#include "Request.hpp"
#include "SpecifyCommand.hpp"

std::vector<Command*> *Request::ParseRequest(Server *server, Client *client, std::string request, int *offset) {
	std::vector<std::string> *message_list = NULL;
	std::vector<Command *> *command_list = NULL;

	log::cout << "Request : " << request << "\n";
	message_list = SplitRequest(request, offset);
	command_list = SplitMessage(server, client, *message_list);

	delete message_list;

	return command_list;
}

std::vector<std::string>	*Request::SplitRequest(const std::string &request, int *offset) {
	std::vector<std::string> *result = new std::vector<std::string>();
	static const std::string	delimiter = "\r\n";
	size_t start = 0, end = 0;

	while ((end = request.find(delimiter, start)) != std::string::npos) {
		result->push_back(request.substr(start, end - start));
		start = end + delimiter.length();
	}
	*offset = start;
	return result;
}

std::vector<Command *>	*Request::SplitMessage(Server *server, Client *client, const std::vector<std::string> &message_list) {
	std::vector<Command *> *command_list = new std::vector<Command *>();
	std::vector<std::string>	token_list;
	std::string msg;
	Command	*com;

	for (size_t i = 0; i < message_list.size(); ++i) {
		if (message_list[i][0] == ' ')
			continue ;
		msg = RemoveDuplicateSpace(message_list[i]);
		token_list.clear();
		SeperateWhiteSpace(msg, &token_list);
		com = CommandFactory(token_list, server, client);
		command_list->push_back(com);
	}
	return command_list;
}

// Message can be seperated one or more whitespace
std::string Request::RemoveDuplicateSpace(const std::string& str) {
	std::string result;
	bool space_flag = false;
	bool colon_flag = false;
	size_t	start = 0;

	while (str[start] && str[start] == ' ')
		++start;

	for (size_t i = start; i < str.size(); ++i) {
		if (str[i] == ' ' && colon_flag == false) {
			if (space_flag == false) {
				result += ' ';
				space_flag = true;
			}
		}
		else if (i != start && str[i] == ':' ) {
			result += str[i];
			colon_flag = true;
		}
		else {
			result += str[i];
			space_flag = false;
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
		if (start < str.length() && str[start] == ':') {
			token_list->push_back(str.substr(start + 1));
			return ;
		}
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
	return acc % static_cast<size_t>(INTMAX);
}

int	Request::SearchCommand(const std::vector<std::string> &token_list) {
	int	acc = 0;

	// first token is <prefix> and second is <command>
	if (token_list[0][0] == ':' && token_list.size() > 1)
		acc = BaseAlphaToNumber(token_list[1]);
	else
		acc = BaseAlphaToNumber(token_list[0]);
	return acc;
}

Command	*Request::CommandFactory(const std::vector<std::string> &token_list, Server *s, Client *c) {
	Command *com = NULL;

	switch (SearchCommand(token_list)) {
		case CAP:
			com = new CapCommand(token_list, s, c);
			break ;
		case JOIN:
			com = new JoinCommand(token_list, s, c);
			break ;
		case KICK:
			com = new KickCommand(token_list, s, c);
			break ;
		case MODE:
			com = new ModeCommand(token_list, s, c);
			break ;
		case NICK:
			com = new NickCommand(token_list, s, c);
			break ;
		case PING:
			com = new PingCommand(token_list, s, c);
			break ;
		case PASS:
			com = new PassCommand(token_list, s, c);
			break ;
		case PART:
			com = new PartCommand(token_list, s, c);
			break ;
		case USER:
			com = new UserCommand(token_list, s, c);
			break ;
		case QUIT:
			com = new QuitCommand(token_list, s, c);
			break ;
		case TOPIC:
			com = new TopicCommand(token_list, s, c);
			break ;
		case WHOIS:
			com = new WhoisCommand(token_list, s, c);
			break ;
		case INVITE:
			com = new InviteCommand(token_list, s, c);
			break ;
		case PRIVMSG:
			com = new PrivmsgCommand(token_list, s, c);
			break ;
		default:
			com = new UnvalidCommand(token_list, s, c);
	}
	return com;
}
