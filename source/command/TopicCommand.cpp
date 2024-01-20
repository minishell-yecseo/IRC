#include "TopicCommand.hpp"

TopicCommand::TopicCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	TopicCommand::AnyOfError(void) {
    return "";
}

void	TopicCommand::Run(void) {
}