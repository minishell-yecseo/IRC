#include "Command.hpp"
#include <vector>

Command::Command(const std::vector<std::string> &token_list)
{
	token_list.size();
	//std::cout << token_list.length() << "Need init of command\n";
	/*
	if (token_list[0][0] == ':')
	{
		this->prefix_ = token_list[0];
		this->command_ = token_list[1];
	}
	else
	{
		this->command_ = token_list[0];
		this->params_ = &token_list[1];
	}
	*/
}

CapCommand::CapCommand(const std::vector<std::string> &token_list) : Command(token_list)
{
	;
}

void	CapCommand::Run()
{

	;
}
