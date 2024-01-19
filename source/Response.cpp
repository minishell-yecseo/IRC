#include "Response.hpp"

std::string	MakeFormat(const std::string& prefix, const std::string& numeric_reply, const std::vector<std::string>& params)
{
	std::string	response;

	if	(prefix.empty() == false) {
		response += prefix + " "
	}
	if (numeric_reply.empty() == false) {
		response += numeric_reply + " "
	}
	for (size_t i = 0; i < params.size(); ++i) {
		response += params[i];
		if (i < params.size() - 1){
			response += " "
		}
	}
	response += CRLF;
	return response.str();
}
