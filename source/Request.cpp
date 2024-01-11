#include "Request.hpp"

Request::Request(std::string request)
{
	SplitRequest(request);
}

void	Request::SplitRequest(std::string request)
{
	static std::string	delimiter = "\r\n";
    size_t start = 0, end = 0;
    while ((end = request.find(delimiter, start)) != std::string::npos) 
	{
        this->message_list_.push_back(request.substr(start, end - start));
        start = end + delimiter.length();
    }
	// Need log file
	if (start != request.length())
		std::cerr << "Unvalid message format\n";
    //this->message_list_.push_back(request.substr(start));
	SplitMessage();
}

// if message_list.size() == 0 case not happend anything
void	Request::SplitMessage(void)
{
	std::vector<std::string>	&message_list = this->message_list_;
    std::string token;

	for (size_t i = 0; i < message_list.size(); ++i)
	{
    	std::stringstream ss(RemoveDuplicateSpace(message_list[i]));
		std::vector<std::string>	token_list;
		// first token can be whitespace
		while (std::getline(ss, token, ' '))
		{
			std::cout << "Token : " << token << "\n";
			token_list.push_back(token);
		}
		ParseRequest(token_list);
    }
}

// Message can be seperated one or more whitespace
std::string Request::RemoveDuplicateSpace(const std::string& str)
{
    std::string result;
    bool isSpace = false;

    for (size_t i = 0; i < str.size(); ++i)
	{
        if (str[i] == ' ')
		{
            if (isSpace == false)
			{
                result += ' ';
                isSpace = true;
            }
        }
		else
		{
            result += str[i];
            isSpace = false;
        }
    }
    return result;
}

int	Request::AlphaBaseNumber(const std::string &token)
{
	int		index = 0;
	size_t	acc = 0;

	while (token[index])
	{
		if (token[index] >= 'a' && token[index] <= 'z')
		{
			acc *= 26;
			acc += token[index] - 'a' + 1;
		}	
		else if (token[index] >= 'A' && token[index] <= 'Z')
		{
			acc *= 26;
			acc += token[index] - 'A' + 1;
		}
		else
			return 0;
		++index;
	}
	return acc % INTMAX;
}

void	Request::ParseRequest(const std::vector<std::string> &token_list)
{
	int	acc = 0;

	if (token_list[0][0] == ':')
		;
	else
	{
		acc = AlphaBaseNumber(token_list[0]);
		switch (acc)
		{
			case JOIN:
				std::cout << "JOIN IN\n";
				break ;
			case KICK:
				std::cout << "KICK IN\n";
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
			case PARK:
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
			case NOTICE:
				std::cout << "NOTICE IN\n";
				break ;
			case PRIVMSG:
				std::cout << "PRIVMSG IN\n";
				break ;
			default:
				std::cout << "Command not found :" << token_list[0] << "\n";
		}
	}
}

/*
int main()
{
	Request	a("            HOBK          abc cde\r\n");
	Request	b("PONG 123\r\nPING 456\r\n");
	Request	c("PASS\r\n");
	Request	d("NICK\r\n");
	Request	e("USER\r\n");
	Request	f("JOIN\r\n");
	Request	g("QUIT\r\n");
	Request	h("PRIVMSG\r\n");
	Request	i("KICK\r\n");
	Request	j("PARK\r\n");
	Request	k("TOPIC\r\n");
	Request	p("NOTICE\r\n");
	Request	q("a\r\n");
	Request	z("ab\r\n");
	return 0;
}
*/
