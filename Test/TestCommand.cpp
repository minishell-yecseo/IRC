#include "TestCommand.hpp"

TestCommand::~TestCommand(void){
}

TestCommand::TestCommand(Server *s, Client *c) {
    this->dummy_server_ = s;
    this->dummy_client_ = c;
}

const std::string&	TestCommand::RunAndReturnRespInTest(Command *command)
{
	command->Run();
	return (command->get_response());
}


bool TestCommand::LockClientMutex(const int& sock) {
	return this->dummy_server_->LockClientMutex(sock);
}

void TestCommand::UnlockClientMutex(const int& sock) {
	this->dummy_server_->UnlockClientMutex(sock);
}

bool	TestCommand::AddChannelMutex(const std::string& name) {
	return this->dummy_server_->AddChannelMutex(name);
}

bool	TestCommand::DeleteChannelMutex(const std::string& name) {
	return this->dummy_server_->DeleteChannelMutex(name);
}

bool	TestCommand::LockChannelMutex(const std::string& name) {
	return this->dummy_server_->LockChannelMutex(name);
}

void	TestCommand::UnlockChannelMutex(const std::string& name) {
	this->dummy_server_->UnlockChannelMutex(name);
}

bool	TestCommand::LockChannelListMutex(void) {
	return this->dummy_server_->LockChannelListMutex();
}

void	TestCommand::UnlockChannelListMutex(void) {
	this->dummy_server_->UnlockChannelListMutex();
}

Channel	*TestCommand::DeleteChannel(const std::string& channel_name) {
	return this->dummy_server_->DeleteChannel(channel_name);
}

void	TestCommand::CeaseChannel(const std::string& channel_name) {
	this->dummy_server_->CeaseChannel(channel_name);
}

bool	TestCommand::SearchChannelByName(const std::string& name) {
	return this->dummy_server_->SearchChannelByName(name);
}

std::string		TestCommand::SearchClientBySock(const int& sock) {
	return this->dummy_server_->SearchClientBySock(sock);
}

int		TestCommand::SearchClientByNick(const std::string& nick) {
	return this->dummy_server_->SearchClientByNick(nick);
}

void	TestCommand::AddDeleteClient(const int& sock) {
	this->dummy_server_->AddDeleteClient(sock);
}

void	TestCommand::CreateChannel(const channel_info& info) {
	this->dummy_server_->CreateChannel(info);
}

bool	TestCommand::AddChannel(Channel *channel) {
	return this->dummy_server_->AddChannel(channel);
}

bool	TestCommand::AddClient(Client *client) {
	return this->dummy_server_->AddClient(client);
}

Client	*TestCommand::DeleteClient(const int& sock) {
	return this->dummy_server_->DeleteClient(sock);
}

std::map<std::string, Channel*>& TestCommand::get_channels(void) {
	return this->dummy_server_->get_channels();
}

Channel	*TestCommand::get_channel_ptr(const std::string& name) {
	return this->dummy_server_->get_channel_ptr(name);
}