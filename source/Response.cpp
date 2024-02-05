#include "Response.hpp"

Response::Response(void) : buffer_("") {
}

Response& Response::operator = (const std::string& str) {
	this->buffer_ = str;
	return *this;
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
	std::stringstream ss;
	ss << i;

	buffer_ += ss.str();
	return *this;
}

Response& Response::operator << (const char c) {
	buffer_ += c;
	return *this;
}

Response& Response::operator + (const char * char_ptr) {
	buffer_ += char_ptr;
	return *this;
}

Response& Response::operator + (const std::string& str) {
	buffer_ += str;
	return *this;
}

Response& Response::operator + (const int i) {
	std::stringstream ss;
	ss << i;

	buffer_ += ss.str();
	return *this;
}

Response& Response::operator + (const char c) {
	buffer_ += c;
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

bool	Response::IsError(void) {
	return buffer_.size() != 0;
}
