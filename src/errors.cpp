#include "stdafx.h"
#include "errors.h"

Exception::Exception() {}

Exception::Exception(std::string const& msg) : message(msg) {
	std::cerr << message << std::endl;
}

const char* Exception::what() const noexcept {
	return message.c_str();
}
