#include "TestCommand.hpp"

TestCommand::~TestCommand(void){
}

TestCommand::TestCommand(Server *s, Client *c) {
    this->dummy_server_ = s;
    this->dummy_client_ = c;
}

const std::string&	TestCommand::RunAndReturnRespInTest(Command *command)
{
    if (command != NULL) {
        command->Run();
        return (command->get_response());
    } else
        return "";
}