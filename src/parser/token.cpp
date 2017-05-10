#include "token.hpp"
#include <sstream>

using namespace std;

bool operator == (const token &tok1, const token &tok){
	return tok1.type == tok.type
			&& tok1.value == tok.value
			&& tok1.parts == tok.parts;
}

string token::to_string() const {
	ostringstream str;
	str << "[" << getTokenNameByType(type) << ", " << value << "]";
	if (hasParts()){
		str << "\nparts(" << parts.size() << "):\n";
		for (auto &t : parts){
			str << t.to_string() << "\n";
		}
	}
	return str.str();
}
