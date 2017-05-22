cmake_minimum_required(VERSION 3.4)
project(configen)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/parser/synparser/interpreter.hpp
        src/parser/synparser/lexer.hpp
        src/parser/synparser/lexer_expression.hpp
        src/parser/synparser/parser.hpp
        src/parser/interpreter_configen.cpp
        src/parser/interpreter_configen.hpp
        src/parser/lexems.cpp
        src/parser/lexems.hpp
        src/parser/lexer_configen.cpp
        src/parser/lexer_configen.hpp
        src/parser/parser_configen.cpp
        src/parser/parser_configen.hpp
        src/parser/token.cpp
        src/parser/token.hpp
        src/main.cpp)

set(SOURCE_FILES_TESTS
        src/parser/synparser/interpreter.hpp
        src/parser/synparser/lexer.hpp
        src/parser/synparser/lexer_expression.hpp
        src/parser/synparser/parser.hpp
        src/parser/interpreter_configen.cpp
        src/parser/interpreter_configen.hpp
        src/parser/lexems.cpp
        src/parser/lexems.hpp
        src/parser/lexer_configen.cpp
        src/parser/lexer_configen.hpp
        src/parser/parser_configen.cpp
        src/parser/parser_configen.hpp
        src/parser/token.cpp
        src/parser/token.hpp
        tests/catch.hpp
        tests/catchconfig.hpp
        tests/interpreter.cpp
        tests/lexer.cpp
        tests/main.cpp
        tests/parser.cpp)

include_directories(src)

add_executable(configen ${SOURCE_FILES})
add_executable(configen_tests ${SOURCE_FILES_TESTS})
