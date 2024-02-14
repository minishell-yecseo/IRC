#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

#include "Command.hpp"
#include "Response.hpp"

class JoinCommand: public Command {
	private:
		JoinCommand(const std::vector<std::string> &token_list, Server *s, Client *c);
		void	Run(void);
		void	AnyOfError(void);
		bool	IsValidChannelInfo(const int& idx);
		void	ParseParam(void);
		bool	IsChannelString(const std::string &str);
		void	Join(const int& idx);
		void	GetChannelInfo(channel_info *info);
		void	CreateChannel(channel_info *info);
		bool	JoinErrorCheck(const channel_info& info);
		bool	TryJoin(const channel_info& inf);
		void	GetSenderInfo(void);
		bool	SendNotifyToMember(const channel_info& info);
		void	SendTopic(const channel_info& info);
		void	SendMemberList(const channel_info& info);
		void	AddChannelForClient(const std::string& chname);

		std::string					sender_nick_;
		std::string					sender_host_name_;
		std::string					sender_user_name_;
		std::vector<std::string>	channels_;
		std::vector<std::string>	keys_;

		friend class	Request;
		friend class	TestJoinCommand;
};
#endif
