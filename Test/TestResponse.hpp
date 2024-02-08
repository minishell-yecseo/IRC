#ifndef TESTRESPONSE_HPP
# define TESTRESPONSE_HPP

#include <cassert>
#include <cstring>

#include "Request.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "log.hpp"
#include "Mutex.hpp"
#include "Utils.hpp"
#include "ThreadPool.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include "PingCommand.hpp"
#include "PartCommand.hpp"
#include "CapCommand.hpp"
#include "WhoisCommand.hpp"
#include "QuitCommand.hpp"
#include "TopicCommand.hpp"
#include "PrivmsgCommand.hpp"
#include "KickCommand.hpp"
#include "ModeCommand.hpp"
#include "InviteCommand.hpp"
#include "UnvalidCommand.hpp"
#include "NickCommand.hpp"
#include "PassCommand.hpp"

void	TestResponse(Server *s, Client *c);
#endif
