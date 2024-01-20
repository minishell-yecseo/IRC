#include "NoticeCommand.hpp"

NoticeCommand::NoticeCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}
std::string	NoticeCommand::AnyOfError(void) {
    return "";
}

void	NoticeCommand::Run(void) {
}