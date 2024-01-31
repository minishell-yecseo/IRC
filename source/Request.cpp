#include "Request.hpp"
#include "SpecifyCommand.hpp"

std::vector<Command*> Request::ParseRequest(Server *server, Client *client, std::string request, int *offset) {
	std::vector<std::string> message_list;
	std::vector<Command *> command_list;

	log::cout << "Request : " << request << "\n";
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
		log::cout << "Unvalid message format\n";
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
	bool space_flag = false;
	bool colon_flag = false;

	for (size_t i = 0; i < str.size(); ++i) {
		if (str[i] == ' ' && colon_flag == false) {
			if (space_flag == false) {
				result += ' ';
				space_flag = true;
			}
		}
		else if (i != 0 && str[i] == ':' ) {
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

Command	*Request::CommandFactory(const std::vector<std::string> &token_list) {
	Command *c = NULL;

	switch (SearchCommand(token_list)) {
		case CAP:
			c = new CapCommand(token_list);
			break ;
		case JOIN:
			c = new JoinCommand(token_list);
			break ;
		case KICK:
			c = new KickCommand(token_list);
			break ;
		case MODE:
			c = new ModeCommand(token_list);
			break ;
		case NICK:
			c = new NickCommand(token_list);
			break ;
		case PING:
			c = new PingCommand(token_list);
			break ;
		case PASS:
			c = new PassCommand(token_list);
			break ;
		case PART:
			c = new PartCommand(token_list);
			break ;
		case USER:
			c = new UserCommand(token_list);
			break ;
		case QUIT:
			c = new QuitCommand(token_list);
			break ;
		case TOPIC:
			c = new TopicCommand(token_list);
			break ;
		case WHOIS:
			c = new WhoisCommand(token_list);
			break ;
		case INVITE:
			c = new InviteCommand(token_list);
			break ;
		case NOTICE:
			c = new NoticeCommand(token_list);
			break ;
		case PRIVMSG:
			c = new PrivmsgCommand(token_list);
			break ;
		default:
			log::cout << "Command not found : " ;
			for (size_t i = 0; i < token_list.size(); ++i) {
				log::cout << token_list[i] << " ";
			}
			log::cout << "\n";
	}
	return c;
}
