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

TEST_CASE("Interpreter: base", "[Interpreter]"){
	stringstream input;
	stringstream out;

	input << R"---(
test | aa b {
	"${aa} -> ${b}\n"
}

test 0 1
test a b
)---";

	ParserConfigen parser(input);
	LexerConfigen lexer(parser);
	InterpreterConfigen intr(out);

	lexer.nextToken();
	auto blk = lexer.parse();
	intr.run(blk);

	REQUIRE(out.str() == "0 -> 1\na -> b\n");
}
