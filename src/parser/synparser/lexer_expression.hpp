#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <tuple>

#include "lexer.hpp"

namespace synparser {

using namespace std::literals;

template<typename _Token, typename _BaseLexem>
class lexer_expression : public lexer<_Token, _BaseLexem> {
protected:
	using _Base = lexer<_Token, _BaseLexem>;

	using _Base::error;
	using _Base::mparser;
public:
	using typename _Base::lexem_t;
	using typename _Base::token_t;

	using operator_f = std::function<lexem_t(int, lexem_t)>;
	using operator_map = std::unordered_map<int, std::unordered_map<std::string, operator_f> >;
	using operator_t = std::tuple<int, operator_f>;
protected:
	token_t tok;

	operator_map pre_ops;
	operator_map inf_ops;
	operator_map post_ops;

	bool nextToken();
	bool nextTokenWithSpaces();
	operator_t findOperator(const operator_map &ops, int lvl, std::string op);

	lexem_t postfix(int lvl, lexem_t val);
	lexem_t prefix(int lvl);
	lexem_t infix(int lvl);

	lexem_t makeLexemValue(token_t tok) = 0;
	lexem_t makeLexemUnary(token_t op, lexem_t arg) = 0;
	lexem_t makeLexemBinary(token_t op, lexem_t arg1, lexem_t arg2) = 0;
public:
	lexer_expression(parser<token_t> &pars) : _Base(pars){}
	virtual ~lexer_expression(){}

	virtual lexem_t parse() override;
};

template<typename _Token, typename _BaseLexem>
typename lexer_expression<_Token, _BaseLexem>::operator_t lexer_expression<_Token, _BaseLexem>::findOperator(const operator_map &ops, int lvl, std::string op){
	for (auto &l : ops){
		auto &level = l.first;
		if (level >= lvl){
			for (auto &p : l.second){
				if (p.first == op){
					return operator_t {level, p.second};
				}
			}
		}
	}

	return operator_t {-1, nullptr};
}

template<typename _Token, typename _BaseLexem>
typename lexer_expression<_Token, _BaseLexem>::lexem_t lexer_expression<_Token, _BaseLexem>::postfix(int lvl, lexem_t val){
	while (tok.isOp()){
		int oplvl;
		operator_f opf;

		tie(oplvl, opf) = findOperator(post_ops, lvl, tok.value);
		if (oplvl == -1){
			break;
		}

		if (opf != nullptr){
			val = opf(*this, val, oplvl);
		} else {
			val = makeLexemUnary(tok, val);
			nextToken();
		}

		if (tok.isNone()){
			break;
		}
	}

	return val;
}

template<typename _Token, typename _BaseLexem>
typename lexer_expression<_Token, _BaseLexem>::lexem_t lexer_expression<_Token, _BaseLexem>::prefix(int lvl){
	lexem_t val;
	token_t op;

	switch (tok.type){
		case token_t::op:
			op = tok;
			if (op.value == "("){
				if (!nextToken()){
					error("Operand excepted after \"(\"");
				}

				val = infix(1);

				if (!tok.isOp(")")){
					error("Excepted \")\"");
				}

				nextToken();

				return postfix(lvl, val);
			} else {
				int oplvl;
				operator_f opf;

				tie(oplvl, opf) = findOperator(pre_ops, lvl, op.value);
				if (oplvl == -1){
					error("Unexcepted operator \""s + op.value + "\"");
				}

				if (!nextToken()){
					error("Unexcepted end of file. Excepted identificator or expression");
				}

				val = infix(oplvl);

				if (opf != nullptr){
					val = opf(*this, val, oplvl);
				} else {
					val = makeLexemUnary(op, val);
				}

				return postfix(lvl, val);
			}

		case token_t::none:
			error("Unexcepted end of file");
			break;

		default:
			val = makeLexemValue(tok);
			nextToken();

			return postfix(lvl, val);
	}

	return nullptr;
}

template<typename _Token, typename _BaseLexem>
typename lexer_expression<_Token, _BaseLexem>::lexem_t lexer_expression<_Token, _BaseLexem>::infix(int lvl){
	lexem_t a1 = prefix(lvl);

	while (tok.isOp()){
		token_t op = tok;

		int oplvl;
		operator_f opf;

		tie(oplvl, opf) = findOperator(inf_ops, lvl, op.value);

		if (oplvl == -1){
			return a1;
		}

		if (!nextToken()){
			error("Unexcepted end of file. Operator excepted.");
		}

		if (opf != nullptr){
			a1 = opf(*this, a1, oplvl);
		} else {
			lexem_t a2 = infix(oplvl + 1);
			a1 = makeLexemBinary(op, a1, a2);
		}

		a1 = postfix(lvl, a1);
	}

	return a1;
}

template<typename _Token, typename _BaseLexem>
bool lexer_expression<_Token, _BaseLexem>::nextToken(){
	do {
		tok = mparser.next();
	} while (tok.isSpace());
	return !tok.isNone();
}

template<typename _Token, typename _BaseLexem>
bool lexer_expression<_Token, _BaseLexem>::nextTokenWithSpaces(){
	tok = mparser.next();
	return !tok.isNone();
}

}
