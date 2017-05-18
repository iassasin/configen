#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>

#include "synparser/interpreter.hpp"
#include "lexems.hpp"

class InterpreterConfigen : synparser::interpreter<LexemPtr> {
protected:
	InterpreterConfigen *parent = nullptr;
	std::unordered_map<std::string, LexemFunctionPtr> functions;
	std::unordered_map<std::string, std::string> variables;

	LexemFunctionPtr readFunc(std::string name);
	std::string readVar(std::string name);

	void addFunction(std::string name, LexemFunctionPtr func);
	void setVar(std::string name, std::string value);

	inline void setParent(InterpreterConfigen &i){ parent = &i; }
	std::string readValue(lexem_t lex);

	void runBlock(LexemBlockPtr block);
	void runFunc(LexemFunctionPtr func);
	void runFuncCall(LexemFunctionCallPtr fcall);
	void runPrint(LexemPrintPtr print);
public:
	InterpreterConfigen(std::ostream &o) : interpreter<LexemPtr>(o) {}

	virtual void run(lexem_t pgm) override;
};
