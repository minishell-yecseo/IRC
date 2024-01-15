#include "Request.hpp"


std::vector<Command*> Request::ParseRequest(std::string request, int &offset)
{
	std::vector<std::string> message_list;
	std::vector<Command *> command_list;
	
  SplitRequest(request, message_list);
	SplitMessage(message_list, command_list);
	return command_list;
}

void	Request::SplitRequest(const std::string &request, std::vector<std::string> &message_list)
{
	static const std::string	delimiter = "\r\n";
    size_t start = 0, end = 0;

    while ((end = request.find(delimiter, start)) != std::string::npos) 
	{
        message_list.push_back(request.substr(start, end - start));
        start = end + delimiter.length();
    }
	// Need log file
	if (start != request.length())
		std::cerr << "Unvalid message format\n";
}

void	Request::SplitMessage(const std::vector<std::string> &message_list, std::vector<Command *> &command_list)
{
	std::vector<std::string>	token_list;
	std::string msg;
	Command	*c;

	for (size_t i = 0; i < message_list.size(); ++i)
	{
		if (message_list[i][0] == ' ')
			continue ;
    msg = RemoveDuplicateSpace(message_list[i]);
    token_list.clear();
    SeperateWhiteSpace(msg, token_list);
		c = CommandFactory(token_list);
		if (c != NULL)
      command_list.push_back(c);
    }
}

// Message can be seperated one or more whitespace
std::string Request::RemoveDuplicateSpace(const std::string& str)
{
  std::string result;
  bool isSpace = false;
  bool isColon = false;

  for (size_t i = 0; i < str.size(); ++i)
  {
    if (isColon == false && str[i] == ' ')
		{
      if (isSpace == false)
			{
        result += ' ';
        isSpace = true;
      }
    }
		else if (i != 0 && str[i] == ':')
		{
			isColon = true;
		}
		else
		{
      result += str[i];
      isSpace = false;
    }
  }
  return result;
}

// The colon not prefix is mean last parameter and doesn't need remove white spaces
void	Request::SeperateWhiteSpace(const std::string &str, std::vector<std::string> &token_list)
{
	static const char delimiter = ' ';
	size_t	start = 0, end = 0;

	while ((end = str.find(delimiter, start)) != std::string::npos)
	{
		token_list.push_back(str.substr(start, end - start));
		start = end + 1;
		if (start < str.length() && str[start] == ':')
			break ;
	}
	token_list.push_back(str.substr(start));
}

int	Request::BaseAlphaToNumber(const std::string &token)
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
	return acc % static_cast<size_t>(INTMAX);
}

int	Request::SearchCommand(const std::vector<std::string> &token_list)
{
	int	acc = 0;

	// first token is <prefix> and second is <command>
	if (token_list[0][0] == ':' && token_list.size() > 1)
		acc = BaseAlphaToNumber(token_list[1]);
	else
		acc = BaseAlphaToNumber(token_list[0]);

	return acc;
}

Command *	Request::CommandFactory(const std::vector<std::string> &token_list)
{
	Command *c = NULL;

	switch (SearchCommand(token_list))
	{
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
			std::cout << "Command not found : ";
	}
	return c;
}

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <queue>

class ThreadPool
{
	public:
		ThreadPool(int size);
		~ThreadPool();
		std::vector<pthread_t>	threads;
		std::queue<Command*> 	queue;
		pthread_mutex_t			lock;
		pthread_cond_t			notify;
		int	thread_count;
		int	count;
		bool	shutdown;
		static void	*Worker(void *arg);
		void	Enqueue(void *arg);
		
};

ThreadPool::~ThreadPool()
{
	{
		pthread_mutex_lock(&(this->lock));
		this->shutdown = true;
		pthread_cond_broadcast(&(this->notify));
		pthread_mutex_unlock(&(this->lock));
	}

	for (size_t i = 0; i < this->thread_count; ++i)
	{
		pthread_join(this->threads[i], NULL);
	}
	pthread_mutex_destroy(&(this->lock));
	pthread_cond_destroy(&(this->notify));
}

void	ThreadPool::Enqueue(void *arg)
{
	Command	*c;

	c = (Command *)arg;

	pthread_mutex_lock(&(this->lock));
	if (this->shutdown == true)
		return ;
	std::cout << "Enque\n";
	this->queue.push(c);
	this->count += 1;
	pthread_cond_signal(&(this->notify));
	pthread_mutex_unlock(&(this->lock));
}

void	*ThreadPool::Worker(void *arg)
{
	ThreadPool	*pool = (ThreadPool *)arg;
	Command	*c;
	
	for (;;)
	{
		pthread_mutex_lock(&(pool->lock));

		while ((pool->count == 0) && (pool->shutdown == false))
		{
			pthread_cond_wait(&(pool->notify), &(pool->lock));
		}
		if (pool->shutdown == true)
		{
			pthread_mutex_unlock(&(pool->lock));
			break ;
		}

		std::cout << "Run\n";
		c = pool->queue.front();
		pool->queue.pop();
		pool->count -= 1;

		pthread_mutex_unlock(&(pool->lock));
		std::cout << "count : " << pool->count << "\n";
		c->Run();
	}


	pthread_mutex_unlock(&(pool->lock));
	pthread_exit(NULL);
	return NULL;
}

ThreadPool::ThreadPool(int size)
{
	this->thread_count = size;
	pthread_mutex_init(&(this->lock), NULL);
	pthread_cond_init(&(this->notify), NULL);

	this->threads.resize(size);
	for (int i = 0; i < this->thread_count; ++i)
	{
		pthread_create(&(this->threads[i]), NULL,
					ThreadPool::Worker, (void *)this);
	}
}

void a()
{
	ThreadPool pool(4);
	int a = 0;
	std::vector<Command *> v;

	v = Request::ParseRequest("CAP\r\nCAP\r\nCAP\r\nCAP\r\nCAP\r\nCAP\r\nCAP\r\nCAP\r\nCAP\r\nCAP\r\nCAP\r\nCAP\r\nCAP\r\nCAP\r\n", a);
	for (size_t i = 0; i < v.size(); ++i)
	{
		std::cout << "index : " << i << "\n";
		pool.Enqueue(v[i]);
	}
	//usleep(5000);
	return;
}

int main()
{
	a();
	/*
	Request::ParseRequest(":priv PRIVMSG :a asdf asdf s           asdf\r\n:p PRIVMSG :a             b\r\n");
	Request::ParseRequest("PONG 123\r\nPING 456\r\n");
	Request::ParseRequest("PASS\r\n");
	Request::ParseRequest("NICK\r\n");
	Request::ParseRequest("USER\r\n");
	Request::ParseRequest("JOIN\r\n");
	Request::ParseRequest("QUIT\r\n");
	Request::ParseRequest("PRIVMSG\r\n");
	Request::ParseRequest("KICK\r\n");
	Request::ParseRequest("PARK\r\n");
	Request::ParseRequest("TOPIC\r\n");
	Request::ParseRequest("NOTICE\r\n");
	Request::ParseRequest("a\r\n");
	Request::ParseRequest("ab\r\n");
	*/
	return 0;
}

