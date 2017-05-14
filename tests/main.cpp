/*
 * main.cpp
 *
 * Test runner
 *
 *  Created on: 9 мая 2017 г.
 *      Author: assasin
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>

#include "parser/parser_configen.hpp"

#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_COLOUR_NONE
#include "catchconfig.hpp"

using namespace std;
using namespace synparser;

CATCH_TRANSLATE_EXCEPTION( compile_exception& ex ) {
	return string("Compile error(") + to_string(ex.line) + "): " + ex.message;
}

CATCH_STRING_MAKER(token){
	return value.to_string();
}

string readFile(string name){
	ifstream t(name);
	string str;

	t.seekg(0, ios::end);
	str.reserve(t.tellg());
	t.seekg(0, ios::beg);

	str.assign((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
	return str;
}

int main(int argc, char* argv[]){
	int result = Catch::Session().run( argc, argv );
	return result;
}

