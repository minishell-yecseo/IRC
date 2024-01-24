#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <set>

class Client;

#include "Client.hpp"

#define	FT_CH_OPERATOR	(1 << 3)
#define	FT_CH_MEMBER	(1 << 2)
#define	FT_CH_BAN_LIST	(1)

#define	MODE_LIMIT		(1 << 3)
#define MODE_INVITE 	(1 << 2)
#define MODE_TOPIC		(1 << 1)
#define MODE_KEY		(1)
#define CLIENT_LIMIT	9999

class Channel {
	public:
		bool	IsMember(int sock);
		bool	IsOperator(int sock);
		bool	AuthPassword(const std::string& pw);
		bool	Kick(int sock);
		void	Join(int sock);
		void	PromoteMember(int sock);
		void	DegradeMember(int sock);

		/* getter & setter */
		const std::set<int>&	get_members(void);
		const std::set<int>&	get_operators(void);
		const std::set<int>&	get_ban_list(void);
		bool	set_mode(const int& flag, const bool& enable);
		const char&	get_mode(void);
		const std::string&	get_name(void);
		size_t	get_size();
		void	set_topic(const std::string& topic);
		void	set_password(const std::string& password);
		void	set_name(const std::string& name);
		void	set_limit(const int& l);
		void	unset_limit(void);

	private:
		std::string	name_;
		std::string	password_;
		std::string	topic_;
		char	mode_;
		int		limit_;
	
		std::set<int>	members_;
		std::set<int>	operators_;
		std::set<int>	ban_list_;
	};

#endif
