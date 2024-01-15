#include "Command.hpp"

#include <vector>

Command::Command(const std::vector<std::string> &token_list)
{
	size_t	param_index;

	if (token_list[0][0] == ':')
	{
		this->prefix_ = token_list[0];
		this->command_ = token_list[1];
		param_index = 2;
	}
	else
	{
		this->command_ = token_list[0];
		param_index = 1;
	}
	for (size_t index = param_index; index < token_list.size(); ++index)
	{
		this->params_.push_back(token_list[index]);
	}
}

CapCommand::CapCommand(const std::vector<std::string> &token_list) : Command(token_list)
{
	;
}

void	CapCommand::Run()
{

	;
}
