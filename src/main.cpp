/*
 * main.cpp
 *
 *  Created on: 9 мая 2017 г.
 *      Author: assasin
 */

#include <iostream>
#include <fstream>

#include "parser/parser_configen.hpp"
#include "parser/lexer_configen.hpp"
#include "parser/interpreter_configen.hpp"

#define CONFIGEN_VERSION "0.0.1 indev"

using namespace std;

void runFile(string fn){
	ifstream file(fn);

	if (!file){
		cerr << "Error: can't open file '" << fn << "'" << endl;
		return;
	}

	ParserConfigen pars(file);
	LexerConfigen lex(pars);
	InterpreterConfigen intr(cout);

	try {
		auto code = lex.parse();
		intr.run(code);
	} catch (synparser::compile_exception &e){
		cerr << fn << " (" << e.line << "): Error: " << e.message << endl;
	} catch (synparser::runtime_exception &e){
		cerr << fn << " (" << e.line << "): Error: " << e.message << endl;
	}
}

void printHelp(){
	cout << "configen [flags] <files...> [flags]" << endl
		 << "\t--help - print this help and exit" << endl
		 << "\t--version - print version and exit" << endl
			;
}

void printVersion(){
	cout << "configen " CONFIGEN_VERSION << endl;
}

int main(int argc, char **argv){
	if (argc < 2){
		cout << "No input files. Do nothing." << endl
			<< "For help use --help" << endl;
	}

	for (int i = 1; i < argc; ++i){
		auto &arg = argv[i];
		if (arg == "--help"s){
			printHelp();
			return 0;
		}
		else if (arg == "--version"s){
			printVersion();
			return 0;
		}
		else {
			runFile(arg);
		}
	}

	return 0;
}
