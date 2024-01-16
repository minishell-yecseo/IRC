#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include "Client.hpp"

#define MODE_INVITE (1 << 2)
#define MODE_TOPIC	(1 << 1)
#define MODE_KEY	(1)

class Channel {
	public:
		bool	IsMember(const Client& client);
		bool	IsOperator(const Client& client);
		bool	AuthPassword(const std::string& pw);
		bool	Kick(Client client);
		void	Join(Client client);
		void	PromoteMember(Client client);
		
		/* getter & setter */
		bool	set_mode(const int& flag, const bool& enable);
		const char&	get_mode(void);

	private:
		std::string	name_;
		std::string	password_;
		std::string	topic_;
		char	mode_;
	
		std::set<Client>	members_;
		std::set<Client>	operators_;
		std::set<Client>	limits_;
	};

#endif
