#pragma once

#include <exception>
#include <string>

class Exception : std::exception {
public:
	Exception(const char* what) : mWhat(what) {

	}
	const char* what() const throw() { return mWhat.c_str(); }
protected:
	const std::string mWhat;
};

class NotImplementedException : public Exception {
public:
	NotImplementedException(const char* what = "functionality not yet implemented") : Exception(what) {}
};

