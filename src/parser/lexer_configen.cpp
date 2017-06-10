#include <sstream>

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

	if (tok.isStr()){
		return parseLexemPrint();
	}

	if (tok.isVar()){
		token_t id = tok;
		nextToken();

		if (tok.isId("=")){
			nextToken();
			return parseVarAssignment(id);
		} else {
			error("Excepted = in var definition");
		}
	}

	if (tok.isId()){
		token_t id = tok;
		nextToken();

		if (tok.isId("|")){
			nextToken();
			return parseFunctionDefinition(id);
		} else if (tok.isId("|~")){
			nextToken();
			return parseFunctionDefinitionRegex(id);
		} else {
			return parseFunctionCall(id);
		}
	}

	error(string("Unknown token: ") + tok.to_string());
	return nullptr;
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
	ostringstream rx;
	auto func = make_shared<LexemFunctionRegex>();
	func->fname = id.value;

	bool wasSpace = false;
	while (!(tok.isNone() || tok.isId("{"))){
		if (wasSpace){
			wasSpace = false;
			rx << " ";
		}

		if (tok.isId() || (tok.isStr() && !tok.hasParts())){
			rx << tok.value;
		}
		else if (tok.isSpace()){
			wasSpace = true;
		}
		else if (tok.isTerm()){
			//continue
		}
		else {
			error("Unexcepted token in function definition: " + tok.to_string());
		}

		nextTokenWithSpaces();
	}

	try {
		func->setRegex(rx.str());
	} catch (const regex_error &err){
		error("Error in regex expression: "s + err.what());
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

LexerConfigen::lexem_t LexerConfigen::parseFunctionCall(token_t id){
	auto fcall = make_shared<LexemFunctionCall>();
	fcall->fname = id.value;

	while (!(tok.isNone() || tok.isTerm())){
		auto arg = parseArgument();
		if (!arg){
			error("Unexcepted token: " + tok.to_string());
		}

		fcall->arguments.push_back(arg);

		if (tok.isSpace()){
			nextToken();
		}
	}

	nextToken();

	return fcall;
}

LexerConfigen::lexem_t LexerConfigen::parseVarAssignment(token_t id){
	auto lassign = make_shared<LexemAssign>();
	auto lconcat = make_shared<LexemConcat>();
	lassign->var = make_shared<LexemVar>(id.value);

	while (!(tok.isNone() || tok.isTerm())){
		if (tok.isSpace()){
			nextToken();

			if (!(tok.isNone() || tok.isTerm())){
				lconcat->values.push_back(make_shared<LexemValueString>(" "));
			}
		} else {
			auto arg = parseArgument();
			if (!arg){
				error("Unexcepted token: " + tok.to_string());
			}

			lconcat->values.push_back(arg);
		}
	}
	nextToken();

	if (lconcat->values.size() == 0){
		lassign->value = make_shared<LexemValueString>("");
	}
	else if (lconcat->values.size() == 1){
		lassign->value = lconcat->values[0];
	}
	else {
		lassign->value = lconcat;
	}

	return lassign;
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
