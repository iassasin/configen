#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <exception>

struct token {
	enum Type {
		none = 0,
		space,
		id,
		var,
		op,
		number,
		str,
		term,

		unknown = 255,
	};

	static std::string getTokenNameByType(Type t){
		switch (t){
			case none: return "none";
			case space: return "space";
			case id: return "id";
			case var: return "var";
			case op: return "op";
			case number: return "number";
			case str: return "str";
			case term: return "term";
			default: return "unknown";
		}
	}

	Type type;
	std::string value;
	std::vector<token> parts;

	token(Type type = none, std::string value = ""){
		this->type = type;
		this->value = value;
	}

	token(Type type, std::string value, std::vector<token> parts) : token(type, value){
		this->parts = parts;
	}

	std::string to_string() const;

	inline bool hasParts() const { return !parts.empty(); }
	inline bool isNone(){ return type == none; }
	inline bool isSpace(){ return type == space; }

	inline bool isId(){ return type == id; }
	inline bool isId(std::string val){ return type == id && value == val; }
	inline bool isVar(){ return type == var; }
	inline bool isVar(std::string val){ return type == var && value == val; }
	inline bool isOp(){ return type == op; }
	inline bool isOp(std::string val){ return type == op && value == val; }
	inline bool isNumber(){ return type == number; }
	inline bool isNumber(std::string val){ return type == number && value == val; }
	inline bool isStr(){ return type == str; }
	inline bool isStr(std::string val){ return type == str && value == val; }
	inline bool isTerm(){ return type == term; }
	inline bool isTerm(std::string val){ return type == term && value == val; }
};

bool operator == (const token &tok1, const token &tok);
