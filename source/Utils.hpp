#ifndef UTILS_HPP
# define UTILS_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/errno.h>
#include "Colors.hpp"

# ifndef ERR_WRITE
#  define ERR_WRITE 2
# endif

# ifndef ERR_IRCSERV
#  define ERR_IRCSERV 1
# endif

void error_handling(std::string);
void error_handling(const char *);

#endif
