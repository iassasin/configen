/*
 * interpreter.cpp
 *
 *  Created on: 16 мая 2017 г.
 *      Author: assasin
 */

#include <vector>
#include <sstream>
#include <memory>

#include "parser/parser_configen.hpp"
#include "parser/lexer_configen.hpp"
#include "parser/interpreter_configen.hpp"

#include "catchconfig.hpp"

using namespace std;
using namespace synparser;

static string interpret(istream &input){
	stringstream out;

	ParserConfigen parser(input);
	LexerConfigen lexer(parser);
	InterpreterConfigen intr(out);

	lexer.nextToken();
	auto blk = lexer.parse();
	intr.run(blk);

	return out.str();
}

TEST_CASE("Interpreter: base", "[Interpreter]"){
	stringstream input;

	input << R"---(
test | aa b {
	"$aa -> $b\n"
}

test 0 1
test a b
)---";

	REQUIRE(interpret(input) == "0 -> 1\na -> b\n");
}

TEST_CASE("Interpreter: var assignment", "[Interpreter]"){
	stringstream input;

	input << R"---(
$var1 = hello
$var2 = world

test | var1 {
	"$var1 $var2\n"
}

test "hello, awesome"
)---";

	REQUIRE(interpret(input) == "hello, awesome world\n");
}

TEST_CASE("Interpreter: function overload", "[Interpreter]"){
	stringstream input;

	input << R"---(
test | v1 v2 {
	"2: $v1 $v2\n"
}

test | v1 v2 v3 {
	"3: $v1 $v2 $v3\n"
}

test |~ (\d+):(\d+) {
	"nums: $1 $2\n"
}

test |~ (\d+)[^\s]* {
	"snum: $0\n"
}

test | v1 {
	"1: $v1\n"
}

test 1 2 3
test 1
test 1 2
test fas
test 1:2
)---";

	REQUIRE(interpret(input) == "3: 1 2 3\nsnum: 1\n2: 1 2\n1: fas\nnums: 1 2\n");
}

TEST_CASE("Interpreter: function regex", "[Interpreter]"){
	stringstream input;

	input << R"---(
test |~ (\d+):(\d+) {
	"nums: $1 $2\n"
}

test |~ .* {
	"wildcard: $0\n"
}

test 55:22
test 5522
)---";

	REQUIRE(interpret(input) == "nums: 55 22\nwildcard: 5522\n");
}
