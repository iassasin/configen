#pragma once

#include <string>
#include <iostream>

#include "synparser/parser.hpp"
#include "token.hpp"

class ParserConfigen : public synparser::parser<token> {
protected:
	int cur, curline;

	const static char spaces[];

	int nextchar();
public:
	ParserConfigen(std::istream &strm) : parser(strm){
        cur = EOF;
        curline = 1;
	}

	virtual ~ParserConfigen(){}

	virtual token_t next() override;
	virtual int getCurrentLine() override {
		return curline;
	}
};
