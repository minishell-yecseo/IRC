#include "JoinCommand.hpp"

JoinCommand::JoinCommand(const std::vector<std::string> &token_list, Server *s, Client *c) : Command(token_list, s, c) {
}

/*
ERR_UNKNOWNERROR (400)
ERR_NOSUCHCHANNEL (403)
ERR_BADCHANMASK (476)
ERR_NEEDMOREPARAMS (461)
ERR_BADCHANNELKEY (475)
ERR_INVITEONLYCHAN (473)
*/

void	JoinCommand::AnyOfError(void) {
	if (IsRegistered(this->client_sock_) == false) {
		this->resp_ = (std::string)ERR_UNKNOWNERROR + " " + \
					  this->sender_nick_ + " JOIN : Not registered in Server";
		return;
	}

	// Check channel names
	for (size_t i = 0; i < this->channels_.size(); ++i) {
		std::string	tmp = this->channels_[i];
		if (tmp[0] != '#' && tmp[0] != '&') {
			this->resp_ = ERR_BADCHANMASK;
			return;
		}
		if (tmp.size() < 2 || tmp.size() > 200) {
			this->resp_ = ERR_NOSUCHCHANNEL;
			return;
		}
		if (IsChannelString(tmp) == false) {
			this->resp_ = ERR_NOSUCHCHANNEL;
			return;
		}
	}

	//Check keys
	for (size_t i = 0; i < this->keys_.size(); ++i) {
		std::string	tmp = this->keys_[i];
		if (tmp.size() == 1 && tmp[0] == 'x') {
			this->keys_[i] = "";
			continue;
		}
		for (size_t c = 0; c < tmp.size(); ++c) {
			if (isspace(c)) {
				this->resp_ = (std::string)ERR_UNKNOWNERROR + " : key with whitespace";
				return;
			}
		}
	}

	this->is_success_ = true;
}

void	JoinCommand::Run(void) {
	try {
		GetSenderInfo();
		ParseParam();
		AnyOfError();
		if (this->is_success_ == false) {
			return SendResponse(this->client_sock_, this->resp_.get_format_str());
		}
		for (size_t i = 0; i < this->channels_.size(); ++i) {
			this->is_success_ = false;
			Join(i);
			if (this->is_success_ == false)
				SendResponse(this->client_sock_, this->resp_.get_format_str());
		}
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}

void	JoinCommand::Join(const int& idx) {
	channel_info	info;
	memset(&info, 0, sizeof(info));
	info.name = this->channels_[idx];
	info.join_membership = ' ';
	info.key = (size_t) idx < this->keys_.size() ? this->keys_[idx] : "";
	info.mode = info.key.size() > 0 ? MODE_KEY : 0;

	if (this->server_->SearchChannelByName(info.name) == false)
		CreateChannel(&info);
	else
		GetChannelInfo(&info);//PASS

	if (TryJoin(info) == false)
		return;
	if (SendNotifyToMember(info) == false)
		return;
	if (info.mode & MODE_TOPIC)
		SendTopic(info);
	SendMemberList(info);
	SendResponse(this->client_sock_, this->resp_.get_str());
	AddChannelForClient(info.name);
}

void	JoinCommand::SendMemberList(const channel_info& info) {
	/* basic RPL_NAMREPLY format */
	this->resp_ << (std::string)RPL_NAMREPLY << " " << this->sender_nick_;
	if (!(info.mode & MODE_KEY))
		this->resp_ << " @ ";
	else
		this->resp_ << " = ";
	this->resp_ << info.name << " :";

	if (this->server_->SearchChannelByName(info.name) == false)
		return;

	this->server_->LockChannelListMutex();
	std::map<std::string, Channel>& serv_channels = this->server_->get_channels();
	std::map<std::string, Channel>::iterator	itr = serv_channels.find(info.name);
	this->server_->UnlockChannelListMutex();

	if (itr == serv_channels.end())
		return;

	this->server_->LockChannelMutex(info.name);//lock
	Channel& cur_channel = itr->second;
	std::map<int, char>::const_iterator citr = cur_channel.get_members().begin();

	while (citr != cur_channel.get_members().end()) {
		if (citr->second != ' ')
			this->resp_ << citr->second;
		this->resp_ << this->server_->SearchClientBySock(citr->first) << " ";
		citr++;
	}
	this->server_->UnlockChannelMutex(info.name);//unlock
	
	//"<client> <channel> :End of /NAMES list"
	this->resp_ << CRLF;
	this->resp_ << RPL_ENDOFNAMES << " " << this->sender_nick_ << " " << info.name << CRLF;
	//SendResponse(this->client_sock_, this->resp_.get_format_str());
}

bool	JoinCommand::TryJoin(const channel_info& info) {
	bool	join_succ = false;
	Channel		*ch_ptr = NULL;

	if (JoinErrorCheck(info) == false)
		return false;

	ch_ptr = this->server_->get_channel_ptr(info.name);
	if (ch_ptr == NULL) {
		this->resp_ = ERR_NOSUCHCHANNEL;
		return false;
	}

	this->server_->LockChannelMutex(info.name);
	join_succ = ch_ptr->Join(this->client_sock_, info.join_membership);
	this->server_->UnlockChannelMutex(info.name);

	if (join_succ == false) {
		this->resp_ = (std::string)ERR_CHANNELISFULL + " " + this->sender_nick_ + " " + info.name;
		this->resp_ << " :Cannot join channel (+l)";
		return false;
	}
	return true;
}

bool	JoinCommand::JoinErrorCheck(const channel_info& info) {
	Channel		*ch_ptr = NULL;
	ch_ptr = this->server_->get_channel_ptr(info.name);
	if (ch_ptr == NULL) {
		this->resp_ = ERR_NOSUCHCHANNEL;
		return false;
	}

	if (info.is_member) {
		this->resp_ = (std::string)ERR_UNKNOWNERROR + " :" + this->sender_nick_ + " is already in " + info.name;
	} else if (info.mode & MODE_INVITE) {
		/* SEND message :<client> <channel> :Cannot join channel (+i) */
		/* 1. check invite_list */
		bool is_invited = false;
		this->server_->LockChannelMutex(info.name);
		is_invited = ch_ptr->IsInvited(this->client_sock_);
		this->server_->UnlockChannelMutex(info.name);
		
		if (is_invited == true)
			return true;
	
		this->resp_ = (std::string)ERR_INVITEONLYCHAN + " " + this->sender_nick_ + " " + info.name;
		this->resp_ << " : Cannot join channel (+i)";
	} else if (info.mode & MODE_KEY && info.is_auth == false) {
		/* Auth Failed at key only mode channel */
		/* SEND message : "<client> <channel> :Cannot join channel (+k)" */
		this->resp_ = (std::string)ERR_BADCHANNELKEY + " " + this->sender_nick_ + " " + info.name;
		this->resp_ << " : Cannot join channel (+k)";
		return false;
	} else if (info.is_banned) {
		/* Can't Join because the sender(client) has banned */
		/* SEND message : <client> <channel> :Cannot join channel (+b) */
		this->resp_ = (std::string)ERR_BANNEDFROMCHAN + " " + this->sender_nick_ + " " + info.name;
		this->resp_ << " : Cannot join channel (+b)";
		return false;
	}
	this->is_success_ = true;
	return true;
}

void	JoinCommand::CreateChannel(channel_info *info) {
	Channel	new_ch(info->name);
	if (info->mode & MODE_KEY)
		new_ch.set_mode(MODE_KEY, true);
	new_ch.set_password(info->key);
	new_ch.set_host(this->sender_nick_);
	new_ch.set_host_sock(this->client_sock_);
	info->join_membership = '@';
	info->is_auth = true;
	this->server_->AddChannelMutex(info->name);
	
	this->server_->AddChannel(new_ch);
}

void	JoinCommand::SendTopic(const channel_info& info) {
	this->resp_ << (std::string)RPL_TOPIC << " " << this->sender_nick_ << " " << info.name << " :" << info.topic << CRLF; 
}

bool	JoinCommand::SendNotifyToMember(const channel_info& info) {
	this->resp_ = (std::string)":" + this->sender_nick_ + "!" + this->sender_user_name_;
	this->resp_ << "@" << this->sender_host_name_ << " JOIN" <<  " " << info.name;

	Channel *ch_ptr = this->server_->get_channel_ptr(info.name);
	if (ch_ptr == NULL) {
		this->resp_ = ERR_NOSUCHCHANNEL;
		return false;
	}
	this->server_->LockChannelMutex(info.name);
	const std::map<int, char> &members = ch_ptr->get_members();
	std::map<int, char>::const_iterator	itr = members.begin();
	std::map<int, char>::const_iterator	end_itr = members.end();
	while (itr != end_itr) {
		if (itr->first == this->client_sock_) {
			itr++;
			continue;
		}
		SendResponse(itr->first, this->resp_.get_format_str());
		itr++;
	}
	this->server_->UnlockChannelMutex(info.name);
	this->resp_ << CRLF;
	return true;
}

void	JoinCommand::AddChannelForClient(const std::string& chname) {
	/* this client's channels add this channel */
	this->server_->LockClientMutex(this->client_sock_);
	this->client_->add_channel(chname);
	this->server_->UnlockClientMutex(this->client_sock_);
}

void	JoinCommand::GetSenderInfo(void) {
	this->server_->LockClientMutex(this->client_sock_);
	this->sender_nick_ = this->client_->get_nick();
	this->sender_user_name_ = this->client_->get_user_name();
	this->sender_host_name_ = this->client_->get_host_name();
	this->server_->UnlockClientMutex(this->client_sock_);
}

void	JoinCommand::GetChannelInfo(channel_info *info) {
	Channel	*ch_ptr = this->server_->get_channel_ptr(info->name);
	if (ch_ptr == NULL) {
		this->resp_ = ERR_NOSUCHCHANNEL;
		return;
	}
	
	this->server_->LockChannelMutex(info->name);
	info->is_member = ch_ptr->IsMember(this->client_sock_);
	info->is_auth = ch_ptr->AuthPassword(info->key);
	info->is_banned = ch_ptr->IsBanClient(this->client_sock_);
	info->mode = ch_ptr->get_mode();
	info->topic = ch_ptr->get_topic();
	this->server_->UnlockChannelMutex(info->name);
}

void	JoinCommand::ParseParam(void) {
	// empty case has considered at Run();
	if (this->params_.empty() == true)
		return;
	std::string	channels_str = params_[0];
	std::string	keys_str = params_.size() == 2 ? params_[1] : "";

	// parse channels
	size_t	start = 0;
	for (size_t i = 0; i <= channels_str.size(); ++i) {
		if (i == channels_str.size() || channels_str[i] == ',') {
			size_t	len = (i == channels_str.size()) ? (i - start + 1) : i - start;
			std::string	tmp = channels_str.substr(start, len);
			this->channels_.push_back(tmp);
			start = i + 1;
		}
	}

	// parse keys
	if (keys_str.size() == 0)
		return;
	start = 0;
	for (size_t i = 0; i <= keys_str.size(); ++i) {
		if (i == keys_str.size() || keys_str[i] == ',') {
			size_t	len = (i == channels_str.size()) ? (i - start + 1) : i - start;
			std::string	tmp = keys_str.substr(start, len);
			this->keys_.push_back(tmp);
			start = i + 1;
		}
	}
}

bool	JoinCommand::IsChannelString(const std::string &str) {
	for (size_t i = 0; i < str.size(); ++i) {
		if (str[i] == 0 || str[i] == 7 || str[i] == 20 || str[i] == 12 || str[i] == 15 || str[i] == ',')
			return false;
	}
	return true;
}
