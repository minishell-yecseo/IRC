#include "TopicCommand.hpp"

TopicCommand::TopicCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

/*
ERR_NEEDMOREPARAMS (461)
RPL_NOTOPIC (331)

ERR_NOSUCHCHANNEL (403)
ERR_NOTONCHANNEL (442)
ERR_CHANOPRIVSNEEDED (482)
TOPIC <channel> <topic>
"<client> <channel> <nick> <setat>"
*/

void	TopicCommand::NoticeTopic(const std::string& channel_name, const std::string& topic) {
	std::string	nick = this->server_->SearchClientBySock(this->client_sock_);
	std::string notice = channel_name + " " + nick + " :Set topic " + topic + "." + CRLF;
	std::map<int, std::string>      members;

	this->server_->get_channel_members(members, channel_name, FT_CH_MEMBER);
	this->server_->LockChannelMutex(channel_name);
	for (std::map<int, std::string>::iterator it = members.begin(); it != members.end(); ++it) {
		SendResponse(it->first, notice);
	}
	this->server_->UnlockChannelMutex(channel_name);
}

std::string	TopicCommand::CheckChannel(const std::string& channel_name, const std::string& topic) {
	std::string	dummy;
	std::map<std::string, Channel>::iterator chan;

	this->server_->channels_mutex_.lock();
	chan = this->server_->channels_.find(channel_name);
	if (chan == this->server_->channels_.end()) {
		this->server_->channels_mutex_.unlock();
		return dummy + ERR_NOSUCHCHANNEL + " :No such channel.";
	}
	this->server_->channels_mutex_.unlock();

	this->server_->LockChannelMutex(chan->first);
	if ((chan->second).IsMember(this->client_sock_) == false)
		dummy = dummy + ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel.";
	else if (((chan->second).get_mode() & MODE_TOPIC) == true) {
		if ((chan->second).IsOperator(this->client_sock_) == false)
			dummy = dummy + ERR_CHANOPRIVSNEEDED + " " + channel_name + " :You're not channel operator";
	}
	else 
		chan->second.set_topic(topic);
	this->server_->UnlockChannelMutex(chan->first);
	if (dummy.empty() == false)
		return dummy;
	NoticeTopic(channel_name, topic);
	return dummy;
}

std::string	TopicCommand::AnyOfError(void) {
	std::string	dummy;

	if (this->params_.empty())
		return dummy + ERR_NEEDMOREPARAMS + " :Not enough params";
	if (this->params_.size() == 1)
		return dummy + RPL_NOTOPIC + " :No topic is set";
    return CheckChannel(this->params_[0], this->params_[1]);
}

void	TopicCommand::Run(void) {
	Response	r;

	r << AnyOfError();
	if (r.IsError() == true)
		return SendResponse(this->client_sock_, r.get_format_str());
}
