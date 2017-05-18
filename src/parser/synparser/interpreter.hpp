#pragma once

#include <iostream>
#include <string>
#include <exception>

namespace synparser {

class runtime_exception : public std::exception {
public:
	std::string message;
	int line;

	runtime_exception(std::string msg, int l) : exception() {
		message = msg;
		line = l;
	}
};

template <typename _BaseLexem>
class interpreter {
public:
	using lexem_t = _BaseLexem;
protected:
	std::ostream &out;

	virtual void error(std::string info){ throw runtime_exception(info, -1); }
public:
	interpreter(std::ostream &o) : out(o){}
	virtual ~interpreter(){}

	virtual void run(lexem_t pgm) = 0;
};

}
