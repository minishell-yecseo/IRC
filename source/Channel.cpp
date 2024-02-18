#include "Channel.hpp"
#include "log.hpp"

Channel::Channel(const std::string& name) {
	this->limit_ = CLIENT_LIMIT;
	this->name_ = name;
	this->mode_ = MODE_LIMIT;
}

void	Channel::set_topic(const std::string& topic) {
	this->topic_ = topic;
}

const std::string&	Channel::get_key(void) const {
	return this->key_;
}

void	Channel::set_key(const std::string& key) {
	this->key_ = key;
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

const std::string&	Channel::get_host(void) const {
	return this->host_;
}

const int&	Channel::get_host_sock(void) const {
	return this->host_sock_;
}

int	Channel::Kick(int sock) {
	std::map<int, char>::iterator	it = members_.find(sock);
	if (it != members_.end())
		members_.erase(it);
	return (this->members_.size());
}

bool	Channel::Join(int sock, char prefix) {
	if (IsValidPrefix(prefix) == false)
		return false;

	if (this->members_.size() < (size_t) this->limit_) {
		if (this->members_.find(sock) != this->members_.end())
			return false;
		members_.insert(std::make_pair(sock, prefix));
		if (this->invite_list_.find(sock) != this->invite_list_.end())
			this->invite_list_.erase(sock);
		return true;
	}
	return false;
}

bool	Channel::IsValidPrefix(char c) {
	if (c != '@' && c != ' ')
		return false;
	return true;
}

bool	Channel::Invite(int sock) {
	bool	ret = false;
	if (this->invite_list_.find(sock) == this->invite_list_.end()) {
		this->invite_list_.insert(sock);
		ret = true;
	}
	return ret;
}

void	Channel::Mode(int sock, char prefix) {
	std::map<int, char>::iterator	itr = this->members_.find(sock);
	if (itr == this->members_.end() || IsValidPrefix(prefix) == false)
		return;
	itr->second = prefix;
}

bool	Channel::AuthPassword(const std::string& password) const {
	if (password.compare(key_) == 0)
		return true;
	return false;
}

bool	Channel::IsMember(int sock) const {
	std::map<int, char>::const_iterator	it = this->members_.find(sock);
	if (it != members_.end())
		return true;
	return false;
}

bool	Channel::IsOperator(int sock) const {
	std::map<int, char>::const_iterator	it = this->members_.find(sock);
	if ((it != members_.end()) && (it->second == '@'))
		return true;
	return false;
}

bool	Channel::IsBanClient(int sock) const {
	std::set<int>::iterator	it = this->ban_list_.find(sock);
	if (it == this->ban_list_.end())
		return false;
	return true;
}

bool	Channel::IsInvited(int sock) const {
	if (this->invite_list_.find(sock) == this->invite_list_.end())
		return false;
	return true;
}

/* return true when the mode has changed */
void	Channel::set_mode(const int& flag, const bool& enable) {
	if (!enable)
		mode_ &= ~flag;
	else if (enable)
		mode_ |= flag;
}

const char&	Channel::get_mode(void) const {
	return mode_;
}

const std::string&	Channel::get_name(void) const {
	return name_;
}

void	Channel::set_limit(const int& l) {
	this->limit_ = l;
}

void	Channel::unset_limit(void) {
	this->limit_ = CLIENT_LIMIT;
}

const std::map<int, char>&	Channel::get_members(void) const {
	return this->members_;
};

const std::set<int>&	Channel::get_ban_list(void) const {
	return ban_list_;
};

size_t	Channel::get_size(void) const {
	return this->members_.size();
}

const std::string&	Channel::get_topic(void) const {
	return this->topic_;
}
