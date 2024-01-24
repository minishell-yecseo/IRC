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
	std::string	error;

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
				return (error + ERR_UNKNOWNERROR + " : key with whitespace");
		}
	}
	return "";
}

/*
 * In This Server,
 *		channels don't have client number limit		//CHANLIMIT RPL_ISUPPORT	(005)
 * 		clients also don't have channel number limit	//ERR_TOOMANYCHANNELS		(405)
 */

/*	About The Multiple Channel Arguments
 *		When the user send N number of channels with M(< N) numbers of keys,
 *		The Irssi fill the empty channels's keys as 'x'.
 *		So If the key is exactly same with 'x', We consider this as NO KEY.
 */

void	JoinCommand::Run(void) {
	// It can be empty in params_ because irssi send only 'JOIN'
	if (IsRegistered(this->client_sock_) == false)
		return;

	ParseParam();
	PrintParam();
	std::string	error_message = AnyOfError();

	if (error_message.empty() == false) {
		error_message += CRLF;
		SendResponse(this->client_sock_, error_message);
		return;
	}
	GetSenderNick();//PASS
	JoinChannels();
}

void	JoinCommand::JoinChannels(void) {
	/* channels loop */
	for (size_t i = 0; i < this->channels_.size(); ++i) {
		Join(i);
	}
}

void	JoinCommand::Join(const int& idx) {
	Response		reply;
	channel_info	info;
	info.name = this->channels_[idx];
	info.key = (size_t) idx < this->keys_.size() ? this->keys_[idx] : "";
	info.mode = info.key.size() > 0 ? MODE_KEY : 0;

	if (this->server_->SearchChannelByName(info.name) == false) {
		log::cout << BOLDYELLOW << "JOIN::CreateChannel\n" << RESET;
		CreateChannel(&info);
		AddChannelForClient(info.name);
		return;
	}
	log::cout << BOLDYELLOW << "JOIN::ExistChannel try to join\n" << RESET;

	GetChannelInfo(&info);//PASS
	if (JoinErrorCheck(info) == false) {
		log::cout << BOLDYELLOW << "JOIN::JoinErrorCheck exit\n" << RESET;
		return;
	}
	if (TryJoin(info) == false) {
		log::cout << BOLDYELLOW << "JOIN::TryJoin exit\n" << RESET;
		return;
	}

	std::map<int, std::string>	members;
	this->server_->get_channel_members(&members, info.name, FT_CH_MEMBER);

	/* channel 의 멤버들에게 들어왔다고 메시지 send */
	SendNotifyToMember(&members, info);

	/* channel 이 topic mode -> RPL_TOPIC (332) sender에게 보내기 */
	if (info.mode & MODE_TOPIC)
		SendTopic(info);

	SendMemberList(&members, info);
	AddChannelForClient(info.name);
	log::cout << BOLDYELLOW << "JOIN::ExistChannel SUCCESS\n" << RESET;
}

void	JoinCommand::SendMemberList(std::map<int, std::string> *members, \
						const channel_info& info) {
	Response	notify;
	Response	reply;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//"<client> <symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}"
	//<symbol> notes the status of the channel
	//	("=", 0x3D) - Public channel.
	//	("@", 0x40) - Secret channel (secret channel mode "+s").
	//	("*", 0x2A) - Private channel (was "+p", no longer widely used today).
	//	<nick> is the nickname of a client joined to that channel,
	//	and <prefix> is the highest channel membership prefix that client has in the channel, if they have one. 
	//	The last parameter of this numeric is a list of [prefix]<nick> pairs, delimited by a SPACE character (' ', 0x20).
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	notify << RPL_NAMREPLY << " " << this->sender_nick_;
	if (!(info.mode & MODE_KEY))
		notify << "@";
	else
		notify << "=";
	notify << info.name << " :";

	std::map<int, std::string>::iterator	itr = members->begin();
	while (itr != members->end()) {
		if (itr->second.size() == 0) {
			log::cout << "JOIN ERROR AT SendMemberList : " << itr->first << "\n" << RESET;
			continue;
		}
		reply << notify.get_str() << itr->second;
		SendResponse(this->client_sock_, reply.get_format_str());
		reply.flush();
		itr++;
	}
	//"<client> <channel> :End of /NAMES list"
	reply << this->sender_nick_ << " " << info.name << " : End of /NAMES list";
	reply << RPL_ENDOFNAMES;
	SendResponse(this->client_sock_, reply.get_format_str());
}

bool	JoinCommand::TryJoin(const channel_info& info) {
	log::cout << BOLDYELLOW << "JOIN::TryJoin\n" << RESET;
	/* join try : channel 정보 수정 */
	bool	join_succ = false;
	this->server_->LockChannelMutex(info.name);
	join_succ = this->server_->channels_[info.name].Join(this->client_sock_);
	this->server_->UnlockChannelMutex(info.name);

	if (join_succ == false) {
		/* false caused by client limit in channel */
		/* SEND message : */
		log::cout << BOLDRED << "JOIN FAIL AT TRY JOIN\n";
	}
	return join_succ;
}

bool	JoinCommand::JoinErrorCheck(const channel_info& info) {
	Response	reply;
	/* sender 가 channel에 이미 있다면 return */
	/* channel mode : key, invite 확인 후 에러 리턴 */
	/* channel 에서 금지당했으면 -> 에러 리턴 */
	if (info.is_member) {
		log::cout << BOLDYELLOW << "SENDER ALREADY IN CHANNEL\n";
		reply << ": " << ERR_UNKNOWNERROR << " :" << this->sender_nick_ << " is already in " << info.name;
		SendResponse(this->client_sock_, reply.get_format_str());
		return false;
	}

	if (info.mode & MODE_INVITE) {
		log::cout << BOLDYELLOW << "CHANNEL IN INVITE MODE\n";
		/* Can't Join By 'JOIN' command */
		/* SEND message :<client> <channel> :Cannot join channel (+i) */
		reply << ": " << ERR_INVITEONLYCHAN << " " << this->sender_nick_ << " " << info.name;
		reply << " : Cannot join channel (+i)";
		SendResponse(this->client_sock_, reply.get_format_str());
		return false;
	}

	if (info.mode & MODE_KEY && info.is_auth == false) {
		log::cout << BOLDYELLOW << "KEY IS NOT EQUAL\n";
		/* Auth Failed at key only mode channel */
		/* SEND message : "<client> <channel> :Cannot join channel (+k)" */
		reply << ": " << ERR_BADCHANNELKEY << " " << this->sender_nick_ << " " << info.name;
		reply << " : Cannot join channel (+k)";
		SendResponse(this->client_sock_, reply.get_format_str());
		return false;
	}

	if (info.is_banned) {
		log::cout << BOLDYELLOW << "BANNED\n";
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
	new_ch.set_mode(info->mode, true);
	new_ch.set_topic(info->topic);
	new_ch.set_password(info->key);
	new_ch.Join(this->client_sock_);
	new_ch.PromoteMember(this->client_sock_);
	this->server_->AddChannelMutex(info->name);
	this->server_->channels_.insert(make_pair(info->name, new_ch));

	this->server_->channels_mutex_.unlock();//channels unlock

	std::map<int, std::string>	members;
	members.insert(make_pair(this->client_sock_, this->sender_nick_));
	SendNotifyToMember(&members, *info);
}

void	JoinCommand::SendTopic(const channel_info& info) {
	/* SEND topic to the sender */
	/* message : "<client> <channel> :<topic>" */
	Response	reply;

	reply << ": " << RPL_TOPIC << " " << this->sender_nick_ << " " << info.name << " :" << info.topic; 
	SendResponse(this->client_sock_, reply.get_format_str());
}

void	JoinCommand::SendNotifyToMember(std::map<int, std::string> *members, \
										const channel_info& info) {
	Response notify;

	notify << ":" << this->sender_nick_ << " JOIN" <<  " " << info.name;
	std::map<int, std::string>::iterator	itr = members->begin();
	while (itr != members->end()) {
		SendResponse(itr->first, notify.get_format_str());
		itr++;
	}
}

void	JoinCommand::AddChannelForClient(const std::string& chname) {
	/* this client's channels add this channel */
	this->server_->LockClientMutex(this->client_sock_);
	this->client_->add_channel(chname);
	this->server_->UnlockClientMutex(this->client_sock_);
}

void	JoinCommand::GetSenderNick(void) {
	this->server_->LockClientMutex(this->client_sock_);
	this->sender_nick_ = this->client_->get_nick();
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

// TEST CODE
void	JoinCommand::PrintParam(void) {
	std::string	logging = BLUE;
	for (size_t i = 0; i < this->channels_.size(); ++i) {
		logging += this->channels_[i];
		if (i < this->keys_.size()) {
			logging = logging + ":" + keys_[i];
		}
		logging += "\n";
	}
	logging += RESET;
	log::cout << logging;
}

bool	JoinCommand::IsChannelString(const std::string &str) {
	for (size_t i = 0; i < str.size(); ++i) {
		if (str[i] == 0 || str[i] == 7 || str[i] == 20 || str[i] == 12 || str[i] == 15 || str[i] == ',')
			return false;
	}
	return true;
}
