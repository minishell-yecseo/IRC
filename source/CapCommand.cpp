#include "Command.hpp"

CapCommand::CapCommand(const std::vector<std::string> &token_list) : Command(token_list)
{
}

void	CapCommand::Run()
{
	std::cout << &server_ << "\n";
	std::cout << &client_ << "\n";
}
