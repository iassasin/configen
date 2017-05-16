#include "lexer_configen.hpp"

using namespace std;

bool LexerConfigen::nextToken(){
	do {
		tok = mparser.next();
	} while (tok.isSpace());
	return !tok.isNone();
}

bool LexerConfigen::nextTokenWithSpaces(){
	tok = mparser.next();
	return !tok.isNone();
}

LexemBlockPtr LexerConfigen::parseBlock(){
	LexemBlockPtr res = make_shared<LexemBlock>();

	while (!tok.isNone() && !tok.isId("}")){
		lexem_t l = parseLexem();
		if (l){
			res->statements.push_back(l);
		}
	}

	return res;
}

LexerConfigen::lexem_t LexerConfigen::parseLexem(){
	if (tok.isTerm()){
		nextToken();
		return nullptr;
	}

	if (!tok.isId()){
		if (tok.isStr() || tok.isVar()){
			return parseLexemPrint();
		} else {
			error(string("Unknown token: ") + tok.to_string());
		}
	}

	token_t id = tok;
	nextToken();

	if (tok.isId("|")){
		nextToken();
		return parseFunctionDefinition(id);
	}
	else if (tok.isId("|~")){
		nextToken();
		return parseFunctionDefinitionRegex(id);
	}
	else {
		return parseFunctionCall(id);
	}
}

LexerConfigen::lexem_t LexerConfigen::parseLexemPrint(){
	auto vals = make_shared<LexemConcat>();

	while (!tok.isTerm()){
		auto arg = parseArgument();
		if (arg){
			vals->values.push_back(arg);
		}

		if (tok.isSpace()){
			nextTokenWithSpaces();
			if (!tok.isTerm()){
				vals->values.push_back(make_shared<LexemValueString>(" "));
			}
		}
	}

	if (vals->values.size() == 1){
		return make_shared<LexemPrint>(vals->values[0]);
	}

	return make_shared<LexemPrint>(vals);
}

LexerConfigen::lexem_t LexerConfigen::parseFunctionDefinition(token_t id){
	auto func = make_shared<LexemFunction>();
	func->fname = id.value;

	while (!(tok.isNone() || tok.isId("{"))){
		if (tok.isId()){
			func->arguments.push_back(tok.value);
		}
		else if (tok.isTerm()){
			//continue
		}
		else {
			error("Unexcepted token in function definition: " + tok.to_string());
		}

		nextToken();
	}

	if (!tok.isId("{")){
		error("Excepted { in function definition");
	}

	nextToken();
	func->body = parseBlock();

	if (!tok.isId("}")){
		error("Excepted } at end of function definition");
	}

	nextToken();

	return func;
}

LexerConfigen::lexem_t LexerConfigen::parseFunctionDefinitionRegex(token_t id){
	error("Not supported yet");
	return nullptr;
}

LexerConfigen::lexem_t LexerConfigen::parseFunctionCall(token_t id){
	error("Not supported yet");
	return nullptr;
}

LexerConfigen::lexem_t LexerConfigen::parseVarDefinition(){
	if (!tok.isVar()){
		return nullptr;
	}

	string vname = tok.value;

	nextToken();
	if (!tok.isId("=")){
		error("Excepted = after var name");
	}

	return nullptr;
}

LexerConfigen::lexem_t LexerConfigen::parseArgument(){
	auto res = make_shared<LexemConcat>();
	while (true){
		if (tok.isStr() || tok.isId()){
			if (tok.hasParts()){
				for (token_t t : tok.parts){
					if (t.isStr()){
						res->values.push_back(make_shared<LexemValueString>(t.value));
					}
					else if (t.isVar()){
						res->values.push_back(make_shared<LexemVar>(t.value));
					}
					else {
						error("Unknown token in string literal: " + t.to_string());
					}
				}
			} else {
				res->values.push_back(make_shared<LexemValueString>(tok.value));
			}
		}
		else if (tok.isVar()){
			res->values.push_back(make_shared<LexemVar>(tok.value));
		}
		else {
			break;
		}

		nextTokenWithSpaces();
	}

	if (res->values.size() == 0){
		return nullptr;
	}

	if (res->values.size() == 1){
		return res->values[0];
	}

	return res;
}
