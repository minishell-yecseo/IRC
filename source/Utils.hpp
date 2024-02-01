#ifndef UTILS_HPP
# define UTILS_HPP

#include <sys/errno.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <iostream>

#include "Colors.hpp"
#include "log.hpp"

# ifndef ERR_WRITE
#  define ERR_WRITE 2
# endif

# ifndef ERR_IRCSERV
#  define ERR_IRCSERV 1
# endif

void error_handling(std::string);
void error_handling(const char *);
void HandleSIGPIPE(int sig);

#endif
