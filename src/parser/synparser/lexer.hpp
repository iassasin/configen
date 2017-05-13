#pragma once

#include "parser.hpp"

namespace synparser {

template<typename _Token, typename _BaseLexem>
class lexer {
public:
	using token_t = _Token;
	using lexem_t = _BaseLexem;
protected:
	parser<token_t> &mparser;

	virtual void error(std::string msg){ throw compile_exception(msg, mparser.getCurrentLine()); }
public:
	lexer(parser<token_t> &pars) : mparser(pars){}
	virtual ~lexer(){}

	virtual lexem_t parse() = 0;
};

}
