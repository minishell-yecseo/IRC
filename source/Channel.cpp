#include "Channel.hpp"
#include "log.hpp"

Channel::Channel(void) {
	this->limit_ = CLIENT_LIMIT;
	this->mode_ = 0;
}

Channel::Channel(const std::string& name) {
	this->limit_ = CLIENT_LIMIT;
	this->name_ = name;
	this->mode_ = 0;
}

Channel	Channel::operator=(const Channel& ch) {
	this->name_ = ch.name_;
	this->password_ = ch.password_;
	this->topic_ = ch.topic_;
	this->host_ = ch.host_;
	this->host_sock_ = ch.host_sock_;
	this->mode_ = ch.mode_;
	this->limit_ = ch.limit_;
	this->members_ = ch.members_;
	this->operators_ = ch.operators_;
	this->ban_list_ = ch.ban_list_;
	this->invite_list_ = ch.invite_list_;
	return *this;
}

void	Channel::set_topic(const std::string& topic) {
	this->topic_ = topic;
}

const std::string&	Channel::get_password(void) {
	return this->password_;
}

void	Channel::set_password(const std::string& password) {
	this->password_ = password;
}

void	Channel::set_name(const std::string& name) {
	this->name_ = name;
}

void	Channel::set_host(const std::string& host) {
	this->host_ = host;
}

void	Channel::set_host_sock(const int& sock) {
	this->host_sock_ = sock;
}

const std::string&	Channel::get_host(void) {
	return this->host_;
}

const int&	Channel::get_host_sock(void) {
	return this->host_sock_;
}

int	Channel::Kick(int sock) {
	std::set<int>::iterator	it = members_.find(sock);
	if (it != members_.end())
		members_.erase(it);
	it = operators_.find(sock);
	if (it != operators_.end())
		operators_.erase(it);
	return (this->members_.size());
}

bool	Channel::Join(int sock) {
	if (this->members_.size() < (size_t) this->limit_) {
		members_.insert(sock);
		if (this->invite_list_.find(sock) != this->invite_list_.end())
			this->invite_list_.erase(sock);
		return true;
	}
	return false;
}

bool	Channel::Invite(int sock) {
	bool	ret = false;
	if (this->invite_list_.find(sock) == this->invite_list_.end()) {
		this->invite_list_.insert(sock);
		ret = true;
	}
	return ret;
}

void	Channel::PromoteMember(int sock) {
	operators_.insert(sock);
}

void	Channel::DegradeMember(int sock) {
	operators_.erase(sock);
}

bool	Channel::AuthPassword(const std::string& password) {
	if (password.compare(password_) == 0)
		return true;
	return false;
}

bool	Channel::IsMember(int sock) {
	std::set<int>::iterator	it = members_.find(sock);
	if (it != members_.end())
		return true;
	return false;
}

bool	Channel::IsOperator(int sock) {
	std::set<int>::iterator	it = operators_.find(sock);
	if (it != operators_.end())
		return true;
	return false;
}

bool	Channel::IsBanClient(int sock) {
	std::set<int>::iterator	it = this->ban_list_.find(sock);
	if (it == this->ban_list_.end())
		return false;
	return true;
}

bool	Channel::IsInvited(int sock) {
	if (this->invite_list_.find(sock) == this->invite_list_.end())
		return false;
	return true;
}

/* return true when the mode has changed */
bool	Channel::set_mode(const int& flag, const bool& enable) {
	if ((mode_ & flag) && !enable) {
		mode_ &= !flag;
		return true;
	} else if (!(mode_ & flag) && enable) {
		mode_ |= flag;
		return true;
	}
	return false;
}

const char&	Channel::get_mode(void) {
	return mode_;
}

const std::string&	Channel::get_name(void) {
	return name_;
}

void	Channel::set_limit(const int& l) {
	this->limit_ = l;
}

void	Channel::unset_limit(void) {
	this->limit_ = CLIENT_LIMIT;
}

const std::set<int>&	Channel::get_members(void) {
	return members_;
};

const std::set<int>&	Channel::get_operators(void) {
	return operators_;
};

const std::set<int>&	Channel::get_ban_list(void) {
	return ban_list_;
};

size_t	Channel::get_size(void) {
	return this->members_.size();
}

const std::string&	Channel::get_topic(void) {
	return this->topic_;
}
