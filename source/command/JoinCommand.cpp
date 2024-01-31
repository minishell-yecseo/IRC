#include "JoinCommand.hpp"

JoinCommand::JoinCommand(const std::vector<std::string> &token_list) : Command(token_list) {
}

/*
ERR_UNKNOWNERROR (400)
ERR_NOSUCHCHANNEL (403)
ERR_BADCHANMASK (476)
ERR_NEEDMOREPARAMS (461)
ERR_BADCHANNELKEY (475)
ERR_INVITEONLYCHAN (473)
*/

std::string	JoinCommand::AnyOfError(void) {
	std::string	dummy;

	if (IsRegistered(this->client_sock_) == false)
		return (dummy + ": " + ERR_UNKNOWNERROR + " " + \
			this->sender_nick_ + " JOIN : Not registered in Server");

	ParseParam();

	// Check channel names
	for (size_t i = 0; i < this->channels_.size(); ++i) {
		std::string	tmp = this->channels_[i];
		if (tmp[0] != '#' && tmp[0] != '&')
			return ERR_BADCHANMASK;
		if (tmp.size() < 2 || tmp.size() > 200)
			return ERR_NOSUCHCHANNEL;
		if (IsChannelString(tmp) == false)
			return ERR_NOSUCHCHANNEL;
	}

	//Check keys
	for (size_t i = 0; i < this->keys_.size(); ++i) {
		std::string	tmp = this->keys_[i];
		if (tmp.size() == 1 && tmp[0] == 'x') {
			this->keys_[i] = "";
		}
		for (size_t c = 0; c < tmp.size(); ++c) {
			if (isspace(c))
				return (dummy + ERR_UNKNOWNERROR + " : key with whitespace");
		}
	}
	return "";
}

void	JoinCommand::Run(void) {
	Response	r;

	try {
		GetSenderInfo();
		r << AnyOfError();
		if (r.IsError() == true)
			return SendResponse(this->client_sock_, r.get_format_str());

		for (size_t i = 0; i < this->channels_.size(); ++i)
			Join(i);
	} catch(std::exception& e) {
		log::cout << BOLDRED << e.what() << RESET << "\n";
	}
}

void	JoinCommand::Join(const int& idx) {
	Response		reply;
	channel_info	info;
	info.name = this->channels_[idx];
	info.key = (size_t) idx < this->keys_.size() ? this->keys_[idx] : "";
	info.mode = info.key.size() > 0 ? MODE_KEY : 0;

	if (this->server_->SearchChannelByName(info.name) == false) {
		CreateChannel(&info);
		AddChannelForClient(info.name);
		return;
	}

	GetChannelInfo(&info);//PASS
	if (JoinErrorCheck(info) == false)
		return;
	if (TryJoin(info) == false)
		return;
	
	SendNotifyToMember(info);
	if (info.mode & MODE_TOPIC)
		SendTopic(info);
	SendMemberList(info);
	AddChannelForClient(info.name);
}

void	JoinCommand::SendMemberList(const channel_info& info) {
	Response	reply;
	
	/* basic RPL_NAMREPLY format */
	reply << RPL_NAMREPLY << " " << this->sender_nick_;
	if (!(info.mode & MODE_KEY))
		reply << " @ ";
	else
		reply << " = ";
	reply << info.name << " :";

	if (this->server_->SearchChannelByName(info.name) == false)
		return;

	this->server_->channels_mutex_.lock();
	std::map<std::string, Channel>& serv_channels = this->server_->get_channels();
	std::map<std::string, Channel>::iterator	itr = serv_channels.find(info.name);
	this->server_->channels_mutex_.unlock();

	if (itr == serv_channels.end())
		return;

	this->server_->LockChannelMutex(info.name);//lock
	Channel& cur_channel = itr->second;
	std::set<int>::const_iterator citr = cur_channel.get_members().begin();

	while (citr != cur_channel.get_members().end()) {
		if (cur_channel.IsOperator(*citr) == true)
			reply << "@"; 
		else
			reply << "%";
		reply << this->server_->SearchClientBySock(*citr) << " ";
		citr++;
	}
	this->server_->UnlockChannelMutex(info.name);//unlock
	
	//"<client> <channel> :End of /NAMES list"
	reply << CRLF;
	reply << RPL_ENDOFNAMES << " " << this->sender_nick_ << " " << info.name;
	SendResponse(this->client_sock_, reply.get_format_str());
}

bool	JoinCommand::TryJoin(const channel_info& info) {
	Response	reply;
	bool	join_succ = false;

	this->server_->LockChannelMutex(info.name);
	join_succ = this->server_->channels_[info.name].Join(this->client_sock_);
	this->server_->UnlockChannelMutex(info.name);

	if (join_succ == false) {
		/* false caused by client limit in channel */
		/* SEND message :<client> <channel> :Cannot join channel (+l) */
		reply << ": " << ERR_CHANNELISFULL << " " << this->sender_nick_ << " " << info.name;
		reply << " :Cannot join channel (+l)";
		SendResponse(this->client_sock_, reply.get_format_str());
	}
	return join_succ;
}

bool	JoinCommand::JoinErrorCheck(const channel_info& info) {
	Response	reply;
	
	if (info.is_member) {
		reply << ": " << ERR_UNKNOWNERROR << " :" << this->sender_nick_ << " is already in " << info.name;
		SendResponse(this->client_sock_, reply.get_format_str());
		return false;
	}

	if (info.mode & MODE_INVITE) {
		/* SEND message :<client> <channel> :Cannot join channel (+i) */
		/* 1. check invite_list */
		bool is_invited = false;
		if (this->server_->LockChannelMutex(info.name) == false) {
			this->server_->UnlockChannelMutex(info.name);
			return false;
		}
		is_invited = this->server_->channels_[info.name].IsInvited(this->client_sock_);
		this->server_->UnlockChannelMutex(info.name);

		if (is_invited == true)
			return true;
	
		reply << ": " << ERR_INVITEONLYCHAN << " " << this->sender_nick_ << " " << info.name;
		reply << " : Cannot join channel (+i)";
		SendResponse(this->client_sock_, reply.get_format_str());
		return false;
	}

	if (info.mode & MODE_KEY && info.is_auth == false) {
		/* Auth Failed at key only mode channel */
		/* SEND message : "<client> <channel> :Cannot join channel (+k)" */
		reply << ": " << ERR_BADCHANNELKEY << " " << this->sender_nick_ << " " << info.name;
		reply << " : Cannot join channel (+k)";
		SendResponse(this->client_sock_, reply.get_format_str());
		return false;
	}

	if (info.is_banned) {
		/* Can't Join because the sender(client) has banned */
		/* SEND message : <client> <channel> :Cannot join channel (+b) */
		reply << ": " << ERR_BANNEDFROMCHAN << " " << this->sender_nick_ << " " << info.name;
		reply << " : Cannot join channel (+b)";
		SendResponse(this->client_sock_, reply.get_format_str());
		return false;
	}
	return true;
}

void	JoinCommand::CreateChannel(channel_info *info) {
	this->server_->channels_mutex_.lock();//channels lock
	Channel	new_ch(info->name);
	if (info->mode & MODE_KEY)
		new_ch.set_mode(MODE_KEY, true);
	new_ch.set_password(info->key);
	new_ch.set_host(this->sender_nick_);
	new_ch.set_host_sock(this->client_sock_);
	new_ch.Join(this->client_sock_);
	new_ch.PromoteMember(this->client_sock_);
	this->server_->AddChannelMutex(info->name);
	this->server_->channels_.insert(make_pair(info->name, new_ch));

	this->server_->channels_mutex_.unlock();//channels unlock

	SendNotifyToMember(*info);
	//New channel is always in -t mode.
	SendMemberList(*info);
}

void	JoinCommand::SendTopic(const channel_info& info) {
	Response	reply;

	reply << ": " << RPL_TOPIC << " " << this->sender_nick_ << " " << info.name << " :" << info.topic; 
	SendResponse(this->client_sock_, reply.get_format_str());
}

void	JoinCommand::SendNotifyToMember(const channel_info& info) {
	Response notify;

	notify << ":" << this->sender_nick_ << "!" << this->sender_user_name_;
	notify << "@" << this->sender_host_name_ << " JOIN" <<  " " << info.name;

	Channel *ch = this->server_->get_channel_ptr(info.name);
	this->server_->LockChannelMutex(info.name);
	std::set<int> members = ch->get_members();
	std::set<int>::iterator	itr = members.begin();
	std::set<int>::iterator	end_itr = members.end();
	while (itr != end_itr) {
		SendResponse(*itr, notify.get_format_str());
		itr++;
	}
	this->server_->UnlockChannelMutex(info.name);
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
	this->server_->LockChannelMutex(info->name);
	info->is_member = this->server_->channels_[info->name].IsMember(this->client_sock_);
	info->is_auth = this->server_->channels_[info->name].AuthPassword(info->key);
	info->is_banned = this->server_->channels_[info->name].IsBanClient(this->client_sock_);
	info->mode = this->server_->channels_[info->name].get_mode();
	info->topic = this->server_->channels_[info->name].get_topic();
	this->server_->UnlockChannelMutex(info->name);
}

void	JoinCommand::ParseParam(void) {
	// empty case has considered at Run();
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
