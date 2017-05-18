#pragma once

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <cstdint>
#include <unordered_map>
#include <initializer_list>

using std::string;

enum class LexemType : int16_t {
	invalid = -1,
	none = 0,

	var,
	valstr,

	block,
	func,
	funccall,
	print,

	assign,
	concat,
};

struct Lexem {
	LexemType type;

	static std::unordered_map<int16_t, string> _names;

	Lexem(LexemType tp) : type(tp){}
	virtual ~Lexem(){}

	LexemType getType(){ return type; }
	string getName(){ return _names[(int16_t) type]; }
	virtual string to_string() = 0;
};

using LexemPtr = std::shared_ptr<Lexem>;
using LexemWPtr = std::weak_ptr<Lexem>;

//Operators

struct LexemUnary : Lexem {
	LexemPtr op;
	LexemUnary(LexemType tp, LexemPtr op) : Lexem(tp), op(op){}

	virtual string to_string(){ return string() + "[" + getName() + ", " + op->to_string() + "]"; }
};

struct LexemBinary : Lexem {
	LexemPtr op1, op2;
	LexemBinary(LexemType tp, LexemPtr op1, LexemPtr op2) : Lexem(tp), op1(op1), op2(op2){}

	virtual string to_string(){ return string() + "[" + getName() + ", " + op1->to_string() + ", " + op2->to_string() + "]"; }
};

struct LexemConcat : Lexem {
	std::vector<LexemPtr> values;

	LexemConcat() : Lexem(LexemType::concat){}
	LexemConcat(std::vector<LexemPtr> &&args) : Lexem(LexemType::concat), values(args){}

	virtual string to_string(){
		std::ostringstream res;
		res << "[" << getName() << ", " << values.size() << "[";
		for (auto l : values){
			res << l->to_string() << "\n";
		}
		res << "]";

		return res.str();
	}
};

//Variables

struct LexemVar : Lexem {
	string varname;
	LexemVar(string varname) : Lexem(LexemType::var), varname(varname){}

	virtual string to_string(){ return string() + "[" + getName() + ", " + varname + "]"; }
};

using LexemVarPtr = std::shared_ptr<LexemVar>;
using LexemVarWPtr = std::weak_ptr<LexemVar>;

//Constants

struct LexemValueString : Lexem {
	string value;
	LexemValueString(string value) : Lexem(LexemType::valstr), value(value){}

	virtual string to_string(){ return string() + "[" + getName() + ", " + value + "]"; }
};

//Block

struct LexemBlock;

using LexemBlockPtr = std::shared_ptr<LexemBlock>;
using LexemBlockWPtr = std::weak_ptr<LexemBlock>;

struct LexemBlock : Lexem {
	std::vector<LexemPtr> statements;

	LexemBlock() : Lexem(LexemType::block){}
	LexemBlock(std::vector<LexemPtr> &&list) : Lexem(LexemType::block), statements(list){}

	virtual string to_string(){
		std::ostringstream res;
		res << "[" << getName() << ", " << statements.size() << "\n";
		for (auto l : statements){
			res << l->to_string() << "\n";
		}
		res << "]";

		return res.str();
	}
};

struct LexemFunction;

using LexemFunctionPtr = std::shared_ptr<LexemFunction>;
using LexemFunctionWPtr = std::weak_ptr<LexemFunction>;

struct LexemFunction : Lexem {
	string fname;
	std::vector<string> arguments;
	LexemBlockPtr body;

	LexemFunction() : Lexem(LexemType::func){}
	LexemFunction(string fn, std::vector<string> &&args, LexemBlockPtr bd) : Lexem(LexemType::func), fname(fn), arguments(args), body(bd){}

	virtual string to_string(){
		std::ostringstream res;
		res << "[" << getName() << ", " << fname << ", " << arguments.size() << "[";
		for (auto s : arguments){
			res << s << ", ";
		}
		res << "], " << (body ? body->to_string() : "null") << "]";

		return res.str();
	}
};

struct LexemFunctionCall;

using LexemFunctionCallPtr = std::shared_ptr<LexemFunctionCall>;
using LexemFunctionCallWPtr = std::weak_ptr<LexemFunctionCall>;

struct LexemFunctionCall : Lexem {
	string fname;
	std::vector<LexemPtr> arguments;

	LexemFunctionCall() : Lexem(LexemType::funccall){}
	LexemFunctionCall(string fn, std::vector<LexemPtr> &&args) : Lexem(LexemType::funccall), fname(fn), arguments(args){}

	virtual string to_string(){
		std::ostringstream res;
		res << "[" << getName() << ", " << fname << ", " << arguments.size() << "[";
		for (auto l : arguments){
			res << l->to_string() << "\n";
		}
		res << "]";

		return res.str();
	}
};

struct LexemPrint;

using LexemPrintPtr = std::shared_ptr<LexemPrint>;
using LexemPrintWPtr = std::weak_ptr<LexemPrint>;

struct LexemPrint : Lexem {
	LexemPtr value;

	LexemPrint() : Lexem(LexemType::print){}
	LexemPrint(LexemPtr val) : Lexem(LexemType::print), value(val){}

	virtual string to_string(){
		std::ostringstream res;
		string vl = value ? value->to_string() : "null";
		res << "[" << getName() << ", " << vl << "]";
		return res.str();
	}
};

struct LexemAssign;

using LexemAssignPtr = std::shared_ptr<LexemAssign>;
using LexemAssignWPtr = std::weak_ptr<LexemAssign>;

struct LexemAssign : Lexem {
	LexemVarPtr var;
	LexemPtr value;

	LexemAssign() : Lexem(LexemType::assign){}
	LexemAssign(LexemVarPtr vr, LexemPtr vl) : Lexem(LexemType::assign), var(vr), value(vl){}

	virtual string to_string(){
		string vs = var ? var->to_string() : "null";
		string vl = value ? value->to_string() : "null";
		return "[" + getName() + ", " + vs + ", " + vl + "]";
	}
};
