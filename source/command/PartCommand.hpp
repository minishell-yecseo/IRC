#ifndef PARTCOMMAND_HPP
#define PARTCOMMAND_HPP

#include "Command.hpp"

class PartCommand: public Command {
	public:
		PartCommand(const std::vector<std::string> &token_list);
		void	Run(void);
		void	AnyOfError(void);
		void	CheckChannel(const std::string& channel_name);
		void	ParseParam(void);
		void	PartEachTarget(void);

	private:
		std::vector<std::string> target_channels_;
};
#endif
