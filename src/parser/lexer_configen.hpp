#pragma once

#include <string>
#include <vector>

#include "lexems.hpp"
#include "parser_configen.hpp"
#include "synparser/lexer.hpp"

class LexerConfigen : public synparser::lexer<token, LexemPtr> {
protected:
	token tok;
public:
	LexerConfigen(synparser::parser<token> &pars) : synparser::lexer<token_t, lexem_t>(pars){}

	~LexerConfigen(){}

	bool nextToken();
	bool nextTokenWithSpaces();

	LexemBlockPtr parseBlock();

	lexem_t parseLexem();
	lexem_t parseLexemPrint();

	lexem_t parseFunctionDefinition(token_t id);
	lexem_t parseFunctionDefinitionRegex(token_t id);
	lexem_t parseFunctionCall(token_t id);

	lexem_t parseVarDefinition();

	lexem_t parseArgument();

	virtual lexem_t parse() override { nextToken(); return parseBlock(); }
};
