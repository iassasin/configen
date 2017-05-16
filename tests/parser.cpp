/*
 * parser.cpp
 *
 *  Created on: 15 мая 2017 г.
 *      Author: assasin
 */

#include <vector>
#include <sstream>

#include "parser/parser_configen.hpp"

#include "catchconfig.hpp"

using namespace std;
using namespace synparser;

CATCH_STRING_MAKER(token){
	return value.to_string();
}

TEST_CASE("Parser: basic", "[Parser]"){
	stringstream str;

	str << R"---(
test | v w {
   data $v; dt dt
	file data.txt
	"("$a")" $b'[]'$c d
}

test "${v2} var ${test}" 1 2 $haha 
)---";

	vector<token> res {
		token(token::term, "\n"),

		token(token::id, "test"),
		token(token::space, " "),
		token(token::id, "|"),
		token(token::space, " "),
		token(token::id, "v"),
		token(token::space, " "),
		token(token::id, "w"),
		token(token::space, " "),
		token(token::id, "{"),
		token(token::term, "\n"),

		token(token::space, "   "),
		token(token::id, "data"),
		token(token::space, " "),
		token(token::var, "v"),
		token(token::term, ";"),

		token(token::space, " "),
		token(token::id, "dt"),
		token(token::space, " "),
		token(token::id, "dt"),
		token(token::term, "\n"),

		token(token::space, "\t"),
		token(token::id, "file"),
		token(token::space, " "),
		token(token::id, "data.txt"),
		token(token::term, "\n"),

		token(token::space, "\t"),
		token(token::str, "("),
		token(token::var, "a"),
		token(token::str, ")"),
		token(token::space, " "),
		token(token::var, "b"),
		token(token::str, "[]"),
		token(token::var, "c"),
		token(token::space, " "),
		token(token::id, "d"),
		token(token::term, "\n"),

		token(token::id, "}"),
		token(token::term, "\n"),

		token(token::term, "\n"),

		token(token::id, "test"),
		token(token::space, " "),
		token(token::str, "", {
			token(token::var, "v2"),
			token(token::str, " var "),
			token(token::var, "test"),
		}),
		token(token::space, " "),
		token(token::id, "1"),
		token(token::space, " "),
		token(token::id, "2"),
		token(token::space, " "),
		token(token::var, "haha"),
		token(token::space, " "),
		token(token::term, "\n"),
	};


	ParserConfigen parser(str);
	int i = 0;
	while(true){
		auto tok = parser.next();
		if (tok.type == token::none){
			break;
		}

		REQUIRE(tok == res[i]);

		++i;
	}

	REQUIRE(i == res.size());
}
