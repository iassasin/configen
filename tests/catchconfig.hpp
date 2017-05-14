/*
 * catchconfig.hpp
 *
 *  Created on: 15 мая 2017 г.
 *      Author: assasin
 */

#pragma once

#include <string>

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

std::string readFile(std::string name);
