#include "interpreter_configen.hpp"

using namespace std;

LexemFunctionPtr InterpreterConfigen::readFunc(string name){
	auto f = functions.find(name);
	if (f != functions.end())
		return f->second;

	return parent ? parent->readFunc(name) : nullptr;
}

string InterpreterConfigen::readVar(string name){
	auto f = variables.find(name);
	if (f != variables.end())
		return f->second;

	return parent ? parent->readVar(name) : "";
}

void InterpreterConfigen::addFunction(string name, LexemFunctionPtr func){
	functions[name] = func;
}

void InterpreterConfigen::setVar(string name, string value){
	variables[name] = value;
}

string InterpreterConfigen::readValue(lexem_t lex){
	switch(lex->getType()){
		case LexemType::valstr: {
			auto l = dynamic_pointer_cast<LexemValueString>(lex);
			return l->value;
		}

		case LexemType::var: {
			auto l = dynamic_pointer_cast<LexemVar>(lex);
			return readVar(l->varname);
		}

		case LexemType::concat: {
			auto lc = dynamic_pointer_cast<LexemConcat>(lex);
			string res;
			for (auto l : lc->values){
				res += readValue(l);
			}
			return res;
		}

		default:
			error("Bad value type: " + lex->getName());
			break;
	}

	return "";
}

void InterpreterConfigen::run(lexem_t pgm){
	switch (pgm->getType()){
		case LexemType::block:
			runBlock(dynamic_pointer_cast<LexemBlock>(pgm));
			break;

		case LexemType::func:
			runFunc(dynamic_pointer_cast<LexemFunction>(pgm));
			break;

		case LexemType::funccall:
			runFuncCall(dynamic_pointer_cast<LexemFunctionCall>(pgm));
			break;

		case LexemType::print:
			runPrint(dynamic_pointer_cast<LexemPrint>(pgm));
			break;

		case LexemType::assign:
			runAssign(dynamic_pointer_cast<LexemAssign>(pgm));
			break;

		default:
			error("Bad instruction: " + pgm->getName());
			break;
	}
}

void InterpreterConfigen::runBlock(LexemBlockPtr block){
	for (auto s : block->statements){
		run(s);
	}
}

void InterpreterConfigen::runFunc(LexemFunctionPtr func){
	addFunction(func->fname, func);
}

void InterpreterConfigen::runFuncCall(LexemFunctionCallPtr fcall){
	auto f = readFunc(fcall->fname);

	if (!f){
		error(string("Function '") + fcall->fname + "' not found");
	}
	if (f->arguments.size() != fcall->arguments.size()){
		error(string("Invalid arguments for function '") + f->fname + "': " + to_string(fcall->arguments.size()) + " passed, but " + to_string(f->arguments.size()) + " excepted");
	}

	InterpreterConfigen scope(out);
	scope.setParent(*this);

	size_t len = f->arguments.size();
	for (size_t i = 0; i < len; ++i){
		scope.setVar(f->arguments[i], readValue(fcall->arguments[i]));
	}

	scope.run(f->body);
}

void InterpreterConfigen::runPrint(LexemPrintPtr print){
	out << readValue(print->value);
}

void InterpreterConfigen::runAssign(LexemAssignPtr assign){
	setVar(assign->var->varname, readValue(assign->value));
}
