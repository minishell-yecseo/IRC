#include "TopicCommand.hpp"

TopicCommand::TopicCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
}

void	TopicCommand::SetTopic(Channel *chan, const std::string& topic) {
	this->is_success_ = true;
	chan->set_mode(MODE_TOPIC, true);
	chan->set_topic(topic);
	NoticeTopic(chan, topic);
}

void	TopicCommand::NoticeTopic(Channel *chan, const std::string& topic) {
	const std::map<int, char>	&member_list = chan->get_members();
	std::string	nick = SearchClientBySock(this->client_sock_);

	this->resp_ = ":" + nick + " TOPIC " + chan->get_name() + " :" + topic;
	for (std::map<int, char>::const_iterator it = member_list.begin(); it != member_list.end(); ++it) {
		SendResponse(it->first, this->resp_.get_format_str());
	}
}

void	TopicCommand::CheckChannel(const std::string& channel_name, const std::string& topic) {
	std::map<std::string, Channel*> *channel_list;
	std::map<std::string, Channel*>::iterator chan;

	LockChannelListMutex();
	channel_list = &(get_channels());
	chan = channel_list->find(channel_name);
	if (chan == channel_list->end()) {
		UnlockChannelListMutex();
		this->resp_ = (std::string)ERR_NOSUCHCHANNEL + " :No such channel";
		return ;
	}
	UnlockChannelListMutex();

	LockChannelMutex(chan->first);
	if ((chan->second)->IsMember(this->client_sock_) == false)
		this->resp_ = (std::string)ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel";
	else if (((chan->second)->get_mode() & MODE_TOPIC)) {
		if ((chan->second)->IsOperator(this->client_sock_) == false)
			this->resp_ = (std::string)ERR_CHANOPRIVSNEEDED + " " + channel_name + " :You're not channel operator";
		else 
			SetTopic(chan->second, topic);
	}
	else 
		SetTopic(chan->second, topic);
	UnlockChannelMutex(chan->first);
}

void	TopicCommand::AnyOfError(void) {
	if (Command::IsRegistered(this->client_sock_) == false)
		this->resp_ = (std::string)ERR_NOTREGISTERED + " :You have not registered";
	else if (this->params_.empty())
		this->resp_ = (std::string)ERR_NEEDMOREPARAMS + " TOPIC :Not enough parameters";
	else if (this->params_.size() == 1)
		this->resp_ = (std::string)RPL_NOTOPIC + " :No topic is set";
	else
		CheckChannel(this->params_[0], this->params_[1]);
}

void	TopicCommand::Run(void) {
	try {
		AnyOfError();
		if (this->is_success_ == false) 
			SendResponse(this->client_sock_, this->resp_.get_format_str());
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
