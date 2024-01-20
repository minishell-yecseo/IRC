#include "PartCommand.hpp"

PartCommand::PartCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	PartCommand::AnyOfError(void) {
    return "";
}

void	PartCommand::Run(void) {
}