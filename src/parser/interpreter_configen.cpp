#include "interpreter_configen.hpp"

using namespace std;

LexemFunctionPtr InterpreterConfigen::findFuncAndApplyArgs(string name, const vector<string> &args, InterpreterConfigen &scope){
	string allargs;
	bool allargsready = false;

	auto fs = functions.find(name);
	if (fs != functions.end()){
		for (auto f : fs->second){
			if (f->getType() == LexemType::func){
				if (f->arguments.size() == args.size()){
					for (size_t i = 0; i < args.size(); ++i){
						scope.setVar(f->arguments[i], args[i]);
					}
					return f;
				}
			}
			else if (f->getType() == LexemType::funcrx){
				if (!allargsready){
					if (args.size() > 0){
						allargs = args[0];
						if (args.size() > 1){
							for (size_t i = 1; i < args.size(); ++i){
								allargs += " ";
								allargs += args[i];
							}
						}
					}
					allargsready = true;
				}

				auto frx = dynamic_pointer_cast<LexemFunctionRegex>(f);
				smatch match;
				if (regex_match(allargs, match, frx->argsrx)){
					for (size_t i = 0; i < match.size(); ++i){
						scope.setVar(to_string(i), match[i].str());
					}
					return frx;
				}
			}
		}
	}

	return parent ? parent->findFuncAndApplyArgs(name, args, scope) : nullptr;
}

void InterpreterConfigen::addFunction(string name, LexemFunctionPtr func){
	auto fs = functions.find(name);
	if (fs == functions.end()){
		functions[name] = {func};
		return;
	}

	if (func->getType() == LexemType::func){
		auto ns = func->arguments.size();
		for (auto it = fs->second.begin(); it != fs->second.end(); ++it){
			if ((*it)->getType() == LexemType::func){
				auto sz = (*it)->arguments.size();
				if (ns > sz){
					fs->second.insert(it, func);
					return;
				} else if (ns == sz){
					error("Function '" + func->fname + "' with " + to_string(ns) + " arguments already defined");
				}
			}
		}
	}
	else if (func->getType() == LexemType::funcrx){
		for (auto it = fs->second.begin(); it != fs->second.end(); ++it){
			if ((*it)->getType() == LexemType::func){
				fs->second.insert(it, func);
				return;
			}
		}
	}
	else {
		error("Unknown function type: " + func->getName());
	}

	fs->second.push_back(func);
}

string InterpreterConfigen::readVar(string name){
	auto f = variables.find(name);
	if (f != variables.end())
		return f->second;

	return parent ? parent->readVar(name) : "";
}

void InterpreterConfigen::setVar(string name, string value){
	variables[name] = value;
}

string InterpreterConfigen::readValue(lexem_t lex){
	switch(lex->getType()){
		case LexemType::valstr: {
			auto l = dynamic_pointer_cast<LexemValueString>(lex);
			return l->value;
		}

		case LexemType::var: {
			auto l = dynamic_pointer_cast<LexemVar>(lex);
			return readVar(l->varname);
		}

		case LexemType::concat: {
			auto lc = dynamic_pointer_cast<LexemConcat>(lex);
			string res;
			for (auto l : lc->values){
				res += readValue(l);
			}
			return res;
		}

		default:
			error("Bad value type: " + lex->getName());
			break;
	}

	return "";
}

void InterpreterConfigen::run(lexem_t pgm){
	switch (pgm->getType()){
		case LexemType::block:
			runBlock(dynamic_pointer_cast<LexemBlock>(pgm));
			break;

		case LexemType::func:
			runFunc(dynamic_pointer_cast<LexemFunction>(pgm));
			break;

		case LexemType::funcrx:
			runFuncRegex(dynamic_pointer_cast<LexemFunctionRegex>(pgm));
			break;

		case LexemType::funccall:
			runFuncCall(dynamic_pointer_cast<LexemFunctionCall>(pgm));
			break;

		case LexemType::print:
			runPrint(dynamic_pointer_cast<LexemPrint>(pgm));
			break;

		case LexemType::assign:
			runAssign(dynamic_pointer_cast<LexemAssign>(pgm));
			break;

		default:
			error("Bad instruction: " + pgm->getName());
			break;
	}
}

void InterpreterConfigen::runBlock(LexemBlockPtr block){
	for (auto s : block->statements){
		run(s);
	}
}

void InterpreterConfigen::runFunc(LexemFunctionPtr func){
	addFunction(func->fname, func);
}

void InterpreterConfigen::runFuncRegex(LexemFunctionRegexPtr func){
	addFunction(func->fname, func);
}

void InterpreterConfigen::runFuncCall(LexemFunctionCallPtr fcall){
	InterpreterConfigen scope(out);

	vector<string> args;
	for (auto arg : fcall->arguments){
		args.push_back(readValue(arg));
	}

	auto f = findFuncAndApplyArgs(fcall->fname, args, scope);

	if (!f){
		error(string("No matching function '") + fcall->fname + "' for call");
	}

	scope.setParent(*this);
	scope.run(f->body);
}

void InterpreterConfigen::runPrint(LexemPrintPtr print){
	out << readValue(print->value);
}

void InterpreterConfigen::runAssign(LexemAssignPtr assign){
	setVar(assign->var->varname, readValue(assign->value));
}
