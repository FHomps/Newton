#pragma once
#include "stdafx.h"

class Exception : public std::exception {
public:
	Exception();
	explicit Exception(std::string const& msg);
	const char* what() const noexcept override;

private:
	std::string message;
};