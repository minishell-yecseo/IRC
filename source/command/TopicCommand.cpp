#include "TopicCommand.hpp"

TopicCommand::TopicCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

void	TopicCommand::NoticeTopic(Channel* c, const std::string& topic) {
	std::string	nick = this->server_->SearchClientBySock(this->client_sock_);
	std::string notice = ":" + nick + " TOPIC " + c->get_name() + " :" + topic + CRLF;
	const std::set<int>	&member_list = c->get_members();

	for (std::set<int>::iterator it = member_list.begin(); it != member_list.end(); ++it) {
		SendResponse(*it, notice);
	}
}

std::string	TopicCommand::CheckChannel(const std::string& channel_name, const std::string& topic) {
	std::string	dummy;
	std::map<std::string, Channel> *channel_list;
	std::map<std::string, Channel>::iterator chan;

	channel_list = &(this->server_->get_channels());
	chan = channel_list->find(channel_name);
	if (chan == channel_list->end())
		return dummy + ERR_NOSUCHCHANNEL + " :No such channel.";

	if ((chan->second).IsMember(this->client_sock_) == false)
		dummy = dummy + ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel.";
	else if (((chan->second).get_mode() & MODE_TOPIC) == true) {
		if ((chan->second).IsOperator(this->client_sock_) == false)
			dummy = dummy + ERR_CHANOPRIVSNEEDED + " " + channel_name + " :You're not channel operator";
	}
	else {
		chan->second.set_mode(MODE_TOPIC, true);
		chan->second.set_topic(topic);
		NoticeTopic(&(chan->second), topic);
	}
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

	try {
		r << AnyOfError();
		if (r.IsError() == true) {
			log::cout << r.get_str();
			return SendResponse(this->client_sock_, r.get_format_str());
		}
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
