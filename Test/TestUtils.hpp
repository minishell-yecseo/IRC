#ifndef TESTUTILS_HPP
# define TESTUTILS_HPP

#include <iostream>
#include <vector>

#ifndef TEST_SERVER_NAME
# define TEST_SERVER_NAME "TEST"
#endif

#ifndef TEST_SERVER_PORT
#define TEST_SERVER_PORT "9090"
#endif

#ifndef TEST_SERVER_PASSWORD
#define TEST_SERVER_PASSWORD "1234"
#endif

#ifndef TEST_CLIENT_NICK
# define TEST_CLIENT_NICK "wooseoki"
#endif

#ifndef TEST_CLIENT_SOCK
# define TEST_CLIENT_SOCK (9)
#endif

#ifndef DUMMY_CLIENT_SOCK
# define DUMMY_CLIENT_SOCK (300)
#endif

#include "Colors.hpp"

void	IsEqual(const std::string& a, const std::string& b);
void	IsStringEqual(const std::string& a, const std::string& b);
void	IsIntEqual(int a, int b);
void	IsVectorEqual(const std::vector<std::string> a, const std::vector<std::string>& b);
void    IsBoolEqual(const bool& a, const bool& b);
#endif
