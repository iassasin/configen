/*
 * lexer.cpp
 *
 *  Created on: 15 мая 2017 г.
 *      Author: assasin
 */

#include <vector>
#include <sstream>
#include <memory>

#include "parser/parser_configen.hpp"
#include "parser/lexer_configen.hpp"

#include "catchconfig.hpp"

using namespace std;
using namespace synparser;

#define START(blk, type) make_shared<blk>(initializer_list<type>{
#define END })
#define LEX(type, ...) make_shared<type>(__VA_ARGS__)

TEST_CASE("Lexer: print", "[Lexer]"){
	stringstream input;
	input << R"---(
'$a b'$c    d
"$a b"$c'd'
)---";

	ParserConfigen parser(input);
	LexerConfigen lexer(parser);

	auto blk = lexer.parse();
	auto res = START(LexemBlock, LexemPtr)
		LEX(LexemPrint,
		START(LexemConcat, LexemPtr)
			START(LexemConcat, LexemPtr)
				LEX(LexemValueString, "$a b"),
				LEX(LexemVar, "c"),
			END,
			LEX(LexemValueString, " "),
			LEX(LexemValueString, "d"),
		END),
		LEX(LexemPrint,
		START(LexemConcat, LexemPtr)
			LEX(LexemVar, "a"),
			LEX(LexemValueString, " b"),
			LEX(LexemVar, "c"),
			LEX(LexemValueString, "d"),
		END),
	END;

	REQUIRE(res->to_string() == blk->to_string());
}

TEST_CASE("Lexer: function", "[Lexer]"){
	stringstream input;
	input << R"---(
test | a b {
	"$a $b"
}

)---";

	ParserConfigen parser(input);
	LexerConfigen lexer(parser);

	auto blk = lexer.parse();
	auto res = START(LexemBlock, LexemPtr)
		LEX(LexemFunction, "test", vector<string>{"a", "b"},
		START(LexemBlock, LexemPtr)
			LEX(LexemPrint,
			START(LexemConcat, LexemPtr)
				LEX(LexemVar, "a"),
				LEX(LexemValueString, " "),
				LEX(LexemVar, "b"),
			END),
		END),
	END;

	REQUIRE(res->to_string() == blk->to_string());
}

TEST_CASE("Lexer: function call", "[Lexer]"){
	stringstream input;
	input << R"---(
test $a 'b' c
test 1 2 3
)---";

	ParserConfigen parser(input);
	LexerConfigen lexer(parser);

	auto blk = lexer.parse();
	auto res = START(LexemBlock, LexemPtr)
		LEX(LexemFunctionCall, "test", vector<LexemPtr> {
			LEX(LexemVar, "a"),
			LEX(LexemValueString, "b"),
			LEX(LexemValueString, "c"),
		}),
		LEX(LexemFunctionCall, "test", vector<LexemPtr> {
			LEX(LexemValueString, "1"),
			LEX(LexemValueString, "2"),
			LEX(LexemValueString, "3"),
		}),
	END;

	REQUIRE(res->to_string() == blk->to_string());
}

TEST_CASE("Lexer: var assignment", "[Lexer]"){
	stringstream input;
	input << R"---(
$a = 5
$b =  the     	test   ;
$c =
)---";

	ParserConfigen parser(input);
	LexerConfigen lexer(parser);

	auto blk = lexer.parse();
	auto res = START(LexemBlock, LexemPtr)
		LEX(LexemAssign, LEX(LexemVar, "a"), LEX(LexemValueString, "5")),
		LEX(LexemAssign, LEX(LexemVar, "b"),
		START(LexemConcat, LexemPtr)
			LEX(LexemValueString, "the"),
			LEX(LexemValueString, " "),
			LEX(LexemValueString, "test"),
		END),
		LEX(LexemAssign, LEX(LexemVar, "c"), LEX(LexemValueString, "")),
	END;

	REQUIRE(res->to_string() == blk->to_string());
}

TEST_CASE("Lexer: function regex", "[Lexer]"){
	stringstream input;
	input << R"---(
test |~ (\d+):(\d+) {
	"$1 $2"
}

)---";

	ParserConfigen parser(input);
	LexerConfigen lexer(parser);

	auto blk = lexer.parse();
	auto res = START(LexemBlock, LexemPtr)
		LEX(LexemFunctionRegex, "test", "(\\d+):(\\d+)",
		START(LexemBlock, LexemPtr)
			LEX(LexemPrint,
			START(LexemConcat, LexemPtr)
				LEX(LexemVar, "1"),
				LEX(LexemValueString, " "),
				LEX(LexemVar, "2"),
			END),
		END),
	END;

	REQUIRE(res->to_string() == blk->to_string());
}

TEST_CASE("Lexer: @use directive", "[Lexer]"){
	stringstream input;
	input << R"---(
@use file
)---";

	ParserConfigen parser(input);
	LexerConfigen lexer(parser);

	auto blk = lexer.parse();
	auto res = START(LexemBlock, LexemPtr)
		LEX(LexemUse, "file"),
	END;

	REQUIRE(res->to_string() == blk->to_string());
}
