#include "Channel.hpp"

Channel::Channel() {
}

bool	Channel::AuthPassword(const std::string& password)
{
	if (password.compare(password_) == 0)
		return true;
	return false;
}

bool	Channel::IsMember(const Client& client)
{
	std::set<Client>::iterator	it = members_.find(client);
	if (it != members_.end())
		return true;
	return false;
}

bool	Channel::IsOperator(const Client& client)
{
	std::set<Client>::iterator	it = operators_.find(client);
	if (it != operators_.end())
		return true;
	return false;
}

/* return true when the mode has changed */
bool	Channel::set_mode(const int& flag, const bool& enable) {
	if ((mode_ & flag) && !enable)
	{
		mode_ &= !flag;
		return true;
	}
	else if (!(mode_ & flag) && enalbe)
	{
		mode_ |= flag;
		return true;
	}
	return false;
}

const char&	Channel::get_mode(void) {
	return mode_;
}
