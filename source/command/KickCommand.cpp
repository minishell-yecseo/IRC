#include "KickCommand.hpp"

KickCommand::KickCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	KickCommand::AnyOfError(void) {
    return "";
}

void	KickCommand::Run(void) {
}