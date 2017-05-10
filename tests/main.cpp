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
#include "catch.hpp"

#define CATCH_TO_STRING(T) \
namespace Catch {\
	std::string toString(T const &value);\
}\
std::string Catch::toString(T const &value)

#define CATCH_STRING_MAKER(T) \
namespace Catch {\
	template<> struct StringMaker<T> {\
		static std::string convert( T const& value );\
	};\
}\
std::string Catch::StringMaker<T>::convert( T const& value )

// CODE

using namespace std;
using namespace synparser;

CATCH_TRANSLATE_EXCEPTION( compile_exception& ex ) {
    return string("Compile error(") + to_string(ex.line) + "): " + ex.message;
}

CATCH_STRING_MAKER(token){
	return value.to_string();
}

TEST_CASE("Parser: basic", "[Parser]"){
	stringstream str;

	str << R"---(
test v w {
   data $v; dt dt
	file data.txt
	"("$a")" $b'[]'$c d
}

test "var ${test}" 1 2 $haha 
)---";

	vector<token> res {
		token(token::term, "\n"),

		token(token::id, "test"),
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
			token(token::str, "var "),
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

