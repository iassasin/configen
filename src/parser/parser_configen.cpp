#include "parser_configen.hpp"

#include <vector>
#include <cstring>

using namespace std;

const char ParserConfigen::spaces[] = " \t";

int ParserConfigen::nextchar(){
	int c = input->get();

	if (c == '\n'){
		++curline;
	}

	return c;
}

ParserConfigen::token_t ParserConfigen::next(){
	if (cur == EOF)
		cur = nextchar();

	if (cur != EOF){
		if (strchr(spaces, cur)){
			string res;

			res += cur;
			cur = nextchar();
			while (cur != EOF && strchr(spaces, cur)){
				res += cur;
				cur = nextchar();
			}

			return token_t(token_t::space, res);
		}
		else if (cur == '\r'){
			cur = nextchar();
			if (cur == '\n'){
				cur = nextchar();
			}

			return token_t(token_t::term, "\n");
		}
		else if (cur == '\n'){
			cur = nextchar();
			if (cur == '\r'){
				cur = nextchar();
			}

			return token_t(token_t::term, "\n");
		}
		else if (cur == ';'){
			cur = nextchar();
			return token_t(token_t::term, ";");
		}
		else if (cur == '\''){
			string res;

			cur = nextchar();
			while (cur != EOF && cur != '\''){
				res += cur;
				cur = nextchar();
			}

			if (cur != '\''){
				error("Unexpected end of string constant");
			}

			cur = nextchar();

			return token_t(token_t::str, res);
		}
		else if (cur == '"'){
			char send = (char) cur;
			string part;
			token_t tok(token_t::str);
			bool escape = false;

			cur = nextchar();
			while (cur != EOF){
				if (escape){
					escape = false;
					switch (cur){
						case 'n': part += '\n'; break;
						case 'r': part += '\r'; break;
						case 't': part += '\t'; break;

						case '\'':
						case '"':
						case '\\':
						case '$':
							part += cur;
							break;


						default:
							part += '\\';
							part += cur;
							break;
					}
					cur = nextchar();
				} else if (cur == send){
					break;
				} else if (cur == '\\'){
					escape = true;
					cur = nextchar();
				} else if (cur == '$'){
					if (!part.empty()){
						tok.parts.push_back(token_t(token_t::str, part));
						part.clear();
					}

					cur = nextchar();
					if (cur == '{'){
						while ((cur = nextchar()) != EOF){
							if (cur == '}')
								break;

							part += cur;
						}

						if (cur != '}'){
							error("Excepted } in string constant (for variable name)");
						}

						cur = nextchar();
					}
					else {
						while (cur != EOF && (cur >= '0' && cur <= '9' || cur >= 'a' && cur <= 'z' || cur >= 'A' && cur <= 'Z')){
							part += cur;
							cur = nextchar();
						}
					}

					if (part.empty()){
						error("Excepted variable name in string constant (but it's empty)");
					}

					tok.parts.push_back(token_t(token_t::var, part));
					part.clear();
				} else {
					part += cur;
					cur = nextchar();
				}
			}

			if (cur != send){
				error("Unexpected end of string constant");
			}

			cur = nextchar();

			if (!part.empty()){
				if (tok.hasParts()){
					tok.parts.push_back(token_t(token_t::str, part));
				} else {
					tok.value = part;
				}
			}

			return tok;
		}
		else if (cur == '$'){
			string res;

			cur = nextchar();
			while (cur != EOF && !strchr(spaces, cur) && !strchr(";\n\r$\"'", cur)){
				res += cur;
				cur = nextchar();
			}

			return token_t(token_t::var, res);
		}
		else {
			string res;

			do {
				res += cur;
				cur = nextchar();
			} while (cur != EOF && !strchr(spaces, cur) && !strchr(";\n\r", cur));

			return token_t(token_t::id, res);
		}
	}

	return token_t(token_t::none);
}
