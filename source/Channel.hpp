#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

class Client;

#include "Client.hpp"
#include "Colors.hpp"
#include "log.hpp"

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
		Channel(const std::string& name);

		bool	IsMember(int sock) const;
		bool	IsOperator(int sock) const;
		bool	IsBanClient(int sock) const;
		bool	IsInvited(int sock) const;
		bool	AuthPassword(const std::string& pw) const;
		int		Kick(int sock);
		bool	Join(int sock, char prefix);
		bool	Invite(int sock);
		void	Mode(int sock, char prefix);//add

		/* getter */
		const std::map<int, char>&	get_members(void) const;
		const std::set<int>&		get_ban_list(void) const;
		const std::string&			get_name(void) const;
		const std::string&			get_topic(void) const;
		const std::string&			get_key(void) const;
		const std::string&			get_host(void) const;
		const char&					get_mode(void) const;
		const int&					get_host_sock(void) const;
		size_t						get_size() const;

		/* setter */
		void	set_mode(const int& flag, const bool& enable);
		void	set_topic(const std::string& topic);
		void	set_key(const std::string& key);
		void	set_name(const std::string& name);
		void	set_limit(const int& l);
		void	set_host(const std::string& host);
		void	set_host_sock(const int& sock);
		void	unset_limit(void);

	private:
		std::string	name_;
		std::string	key_;
		std::string	topic_;
		std::string	host_;
		int		host_sock_;
		char	mode_;
		int		limit_;
	
		std::map<int, char>	members_;
		std::set<int>		ban_list_;
		std::set<int>		invite_list_;

		bool	IsValidPrefix(char);
		
		friend class	Server;
};

typedef struct channel_info {
	Channel		*ch_ptr;
	std::string name;
	std::string	topic;
	std::string	key;
	std::string	host;
	int			host_sock;
	char		mode;
	char		join_membership;
	bool		is_auth;
	bool		is_member;
	bool		is_banned;
} channel_info;

#endif
