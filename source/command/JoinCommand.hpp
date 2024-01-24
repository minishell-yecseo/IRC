#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

#include "Command.hpp"
#include "Response.hpp"

/* function check */
// 1) create channel (no key)									(O)
// 2) create channel (yes key)									(O)
// 3) normal case (no key, no invite, and success for join)		(O)
// 4) normal case (yes key, no invite, and success for join)	(O)
// 5) error case (no key, yes invite, fail)						(O)
// 6) error case (yes key, no invite, and key auth fail)		(O)
// 7) error case (yes key, yes invite, fail)					(O)
// 8) DisconnectClient -> channel delte							(X)
//

typedef struct channel_info {
	std::string name;
	std::string	topic;
	std::string	key;
	char		mode;
	bool		is_auth;
	bool		is_member;
	bool		is_banned;
} channel_info;

class JoinCommand: public Command
{
	public:
		JoinCommand(const std::vector<std::string> &token_list);
		void		Run(void);
		std::string	AnyOfError(void);
		void	ParseParam(void);
		bool	IsChannelString(const std::string &str);
		void	PrintParam(void);

		void	JoinChannels(void);
		void	Join(const int& idx);
		void	GetChannelInfo(channel_info *info);
		void	CreateChannel(channel_info *info);
		bool	JoinErrorCheck(const channel_info& info);
		bool	TryJoin(const channel_info& inf);
		void	GetSenderNick(void);
		void	SendNotifyToMember(std::map<int, std::string> *members, \
									const channel_info& info);
		void	SendTopic(const channel_info& info);
		void	SendMemberList(const channel_info& info);
		void	AddChannelForClient(const std::string& chname);

	private:
		std::string					sender_nick_;
		std::vector<std::string>	channels_;
		std::vector<std::string>	keys_;
};
#endif
