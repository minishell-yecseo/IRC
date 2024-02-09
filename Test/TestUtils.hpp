#ifndef TESTUTILS_HPP
# define TESTUTILS_HPP

#include <iostream>
#include <vector>

#include "Colors.hpp"

void	IsEqual(const std::string& a, const std::string& b);
void	IsStringEqual(const std::string& a, const std::string& b);
void	IsIntEqual(int a, int b);
void	IsVectorEqual(const std::vector<std::string> a, const std::vector<std::string>& b);
#endif
