#include "Response.hpp"

Response::Response(void) : buffer_("") {
}

Response& Response::operator << (const char * char_ptr) {
	buffer_ += char_ptr;
	return *this;
}

Response& Response::operator << (const std::string& str) {
	buffer_ += str;
	return *this;
}

Response& Response::operator << (const int i) {
	std::istringstream	iss(i);
	buffer_ += iss.str();
	return *this;
}

const std::string&	Response::get_str(void) {
	return buffer_;
}

const std::string	Response::get_format_str(void) {
	return (buffer_ + CRLF);
}

const char *	Response::get_chr(void) {
	return buffer_.c_str();
}

void	Response::flush(void) {
	buffer_ = "";
}

size_t	Response::size(void) {
	return buffer_.size();
}
