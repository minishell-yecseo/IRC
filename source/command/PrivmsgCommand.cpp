#include "PrivmsgCommand.hpp"

PrivmsgCommand::PrivmsgCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	PrivmsgCommand::AnyOfError(void) {
    return "";
}

void	PrivmsgCommand::Run(void) {
}