#include "PartCommand.hpp"

PartCommand::PartCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}
/*
ERR_NEEDMOREPARAMS (461)
ERR_NOSUCHCHANNEL (403)
ERR_NOTONCHANNEL (442)
<channel>{,<channel>} [<reason>]
*/

std::string	PartCommand::CheckChannel(const std::string& channel_name) {
	std::string	dummy;
	std::map<std::string, Channel> *channel_list;
	std::map<std::string, Channel>::iterator chan;
	std::set<int>	chan_member_list;
	int	channel_left_num = 1;

	channel_list = &(this->server_->get_channels());
	chan = channel_list->find(channel_name);
	if (chan == channel_list->end())
		return dummy + ERR_NOSUCHCHANNEL + " " + channel_name + " :No such channel.";

	if ((chan->second).IsMember(this->client_sock_) == false)
		dummy = dummy + ERR_NOTONCHANNEL + " " + channel_name + " :You're not on that channel.";
	else {
		// Run KICK here!
		channel_left_num = (chan->second).Kick(this->client_sock_);
		if (channel_left_num)
			chan_member_list = (chan->second).get_members();
	}

	std::string sender = this->server_->SearchClientBySock(this->client_sock_);
	if (channel_left_num == 0)
		this->server_->CeaseChannel(channel_name);

	std::string send_message = dummy + ":" + sender + " PART " + channel_name;
	if (channel_left_num > 0) {
		std::set<int>::const_iterator	iter = chan_member_list.begin();
		SendResponse(*iter, (send_message + CRLF));
	}
	//return dummy + ":" + sender + " PART " + channel_name;
	return send_message;
}

//  :dan-!d@localhost PART #test    ; dan- is leaving the channel #test
void	PartCommand::ParseParam(void) {
	std::string target = this->params_[0];
	size_t	start = 0, end = 0;

	while ((end = target.find(',', start)) != std::string::npos) {
		this->target_channels_.push_back(target.substr(start, end - start));
		start = end + 1;
	}
	this->target_channels_.push_back(target.substr(start));
}

std::string	PartCommand::AnyOfError(void) {
	std::string	dummy;

	if (Command::IsRegistered() == false)
		return dummy + ERR_NOTREGISTERED + " :You have not registered";
	if (this->params_.empty())
		return dummy + ERR_NEEDMOREPARAMS + " :Not enough params";
	return dummy;
}

void	PartCommand::PartEachTarget(Response *r) {
	ParseParam();
	for (size_t i = 0; i < this->target_channels_.size(); ++i) {
		*r << CheckChannel(this->target_channels_[i]);
		SendResponse(this->client_sock_, r->get_format_str());
		r->flush();
	}
}

void	PartCommand::Run(void) {
	Response	r;

	try {
		r << AnyOfError();
		if (r.IsError() == true)
			return SendResponse(this->client_sock_, r.get_format_str());
		PartEachTarget(&r);
	} catch (std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}
