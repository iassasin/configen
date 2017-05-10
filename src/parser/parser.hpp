#pragma once

#include <string>
#include <iostream>
#include <exception>

namespace synparser {

class compile_exception : public std::exception {
public:
	std::string message;
	int line;

	compile_exception(std::string msg, int l) : exception() {
		message = msg;
		line = l;
	}
};

template<typename _Token>
class parser {
public:
	using token_t = _Token;
protected:
	std::istream *input;

	void error(std::string msg){
		throw compile_exception(msg, getCurrentLine());
	}
public:
	parser(std::istream &strm){
        input = &strm;
	}

	virtual ~parser(){}

	virtual token_t next() = 0;
	virtual int getCurrentLine() = 0;
};

}
