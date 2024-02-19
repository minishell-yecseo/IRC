#include "QuitCommand.hpp"

QuitCommand::QuitCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
}

void	QuitCommand::AnyOfError(void) {
    ;
}

void	QuitCommand::Run(void) {
	try {
		this->sender_nick_ = this->server_->SearchClientBySock(this->client_sock_);
		this->resp_ = (std::string)":" + this->sender_nick_ + " QUIT :Quit: Bye for now!";
		SendResponse(this->client_sock_, this->resp_.get_format_str());
		NoticeQuit();
		Command::DisconnectClient();
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}

void	QuitCommand::NoticeQuit(void) {
	this->resp_ = this->sender_nick_ + " QUIT :";
	if (this->params_.size() > 1)
		this->resp_ << params_[1];

	this->server_->LockClientMutex(this->client_sock_);
	std::vector<std::string> channels = this->client_->get_channels();
	std::vector<std::string>::iterator channel_itr = channels.begin();
	Channel *channel_ptr = NULL;
	while (channel_itr != channels.end()) {
		channel_ptr = this->server_->get_channel_ptr(*channel_itr);
		if (channel_ptr != NULL) {
			if (this->server_->LockChannelMutex(*channel_itr)) {
				std::map<int, char> mem = channel_ptr->get_members();
				std::map<int, char>::iterator member_itr = mem.begin();
				while (member_itr != mem.end()) {
					SendResponse(member_itr->first, this->resp_.get_format_str());
					member_itr++;
				}
			}
			this->server_->UnlockChannelMutex(*channel_itr);
		}
		channel_itr++;
	}
	this->server_->UnlockClientMutex(this->client_sock_);
}
