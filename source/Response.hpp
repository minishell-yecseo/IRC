#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <sstream>
#include <string>
#include <vector>

#include "NumericDefine.hpp"
#define CRLF "\r\n"

class Response {
	private:
		std::string	buffer_;

	public:
		Response(void);
		const std::string&	get_str(void);
		const std::string	get_format_str(void);
		const char * 		get_chr(void);
		void				flush(void);
		size_t				size(void);
		bool				empty(void);
		bool				IsError(void);

		Response& operator = (const std::string& str);

		Response& operator << (const char * char_ptr);
		Response& operator << (const std::string& str);
		Response& operator << (const int i);
		Response& operator << (const char c);

		Response& operator + (const char * char_ptr);
		Response& operator + (const std::string& str);
		Response& operator + (const int i);
		Response& operator + (const char c);
};

#endif
