#ifndef COMAND_HPP
#define COMAND_HPP

//#include "Server.hpp"
//#include "Client.hpp"
#include <iostream>
#include <vector>

class Command
{
	public:
		Command(const std::vector<std::string> &token);
		virtual void	Run() = 0;

	protected:
		std::string					prefix_;
		std::string					command_;
		std::vector<std::string>	params_;
		//Server	s_;
		//Client	c_;
};

class CapCommand: public Command
{
	public:
		CapCommand(const std::vector<std::string> &token_list);
		void	Run();
};
#endif
