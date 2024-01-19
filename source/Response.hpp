#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include <vector>

#include "NumericDefine.hpp"
#define CRLF "\r\n"

class Response {
	static std::string	MakeFormat(const std::string& prefix, const std::string& numeric_reply, const std::vector<std::string>& params);
};

#endif
