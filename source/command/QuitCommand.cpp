#include "QuitCommand.hpp"

QuitCommand::QuitCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	QuitCommand::AnyOfError(void) {
    return "";
}

void	QuitCommand::Run(void) {
}