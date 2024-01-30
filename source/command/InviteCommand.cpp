#include "InviteCommand.hpp"

InviteCommand::InviteCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

std::string	InviteCommand::CheckChannel(const std::string& nick, const std::string& channel_name) {
	std::string	dummy;
	int	receiver = this->server_->SearchClientByNick(nick);
	int sender = this->client_sock_;

	if (receiver == FT_INIT_CLIENT_FD)
		return dummy + ERR_NOSUCHNICK + " " + nick + " :No such user";
	std::map<std::string, Channel> *channel_list;
	std::map<std::string, Channel>::iterator chan;

	channel_list = &(this->server_->get_channels());
	chan = channel_list->find(channel_name);
	if (chan == channel_list->end())
		return dummy + ERR_NOSUCHCHANNEL + " " + channel_name + " :No such channel.";

	if ((chan->second).IsMember(sender) == false)
		dummy = dummy + ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel";
	else if ((chan->second).IsOperator(sender) == false)
		dummy = dummy + ERR_CHANOPRIVSNEEDED + " " + channel_name + " :You're not channel operator";
	else if ((chan->second).IsMember(receiver) == true)
		dummy = dummy + ERR_USERONCHANNEL + " " + nick +" " + channel_name + " :is already on cahnnel";
	return dummy;
}

std::string	InviteCommand::AnyOfError(void) {
	std::string	dummy;

	if (Command::IsRegistered() == false)
		return dummy + ERR_NOTREGISTERED + " :You have not registered";
	if (this->params_.size() < 2)
		return dummy + ERR_NEEDMOREPARAMS + " Invite :Not enough parameters";
	return CheckChannel(this->params_[0], this->params_[1]);
}

void	InviteCommand::Run() {
	Response	r;

	try {
		r << AnyOfError();
		if (r.IsError() == true)
			return SendResponse(this->client_sock_, r.get_format_str());
		std::string	sender = this->server_->SearchClientBySock(this->client_sock_);
		int	receiver = this->server_->SearchClientByNick(this->params_[0]);
		if (receiver == FT_INIT_CLIENT_FD)
			return ;
	
		std::string	channel_name = this->params_[1];
		Channel *channel = this->server_->get_channel_ptr(channel_name);
		if (channel != NULL)
			channel->Invite(receiver);
	
		r << ":" << sender <<  " INVITE " << this->params_[0] << " " << this->params_[1] << CRLF;
		SendResponse(receiver, r.get_format_str());
		r.flush();
		r << ":" << sender << " " << RPL_INVITING << " " << this->params_[0] << " " << this->params_[1];
		SendResponse(this->client_sock_, r.get_format_str());
	} catch (std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}

