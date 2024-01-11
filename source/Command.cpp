#include "Command.hpp"

Command::Command(const std::vector<std::string> &token_list)
{
	std::cout << "Need init of command\n";
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
