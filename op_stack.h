#pragma once
#include <stack>
#include <string>

#include "lexical_analyzer.cpp"

void push_typeop(std::string lex);
void check_uno();
void eq_bool();
void eq_int();
void stack_clear();
void stack_del();
void check_bin();

bool is_unary(std::string op);
bool is_assignment(std::string op);
bool is_comparison(std::string op);
bool is_binary(std::string op);
bool is_type(std::string op);
