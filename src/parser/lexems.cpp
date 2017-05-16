#include "lexems.hpp"

std::unordered_map<int16_t, string> Lexem::_names {
	{(int16_t) LexemType::invalid, "(invalid)"},
	{(int16_t) LexemType::none, "(none)"},

	{(int16_t) LexemType::var, "var"},
	{(int16_t) LexemType::valstr, "valstr"},

	{(int16_t) LexemType::block, "block"},
	{(int16_t) LexemType::func, "func"},
	{(int16_t) LexemType::funccall, "funccall"},
	{(int16_t) LexemType::print, "print"},
	{(int16_t) LexemType::assign, "assign"},
	{(int16_t) LexemType::concat, "concat"},
};
