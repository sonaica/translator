#pragma once
#include <stack>
#include <string>

#include "lexical_analyzer.h"

std::string check_id(std::string name);  // its must be in the TID's functions
//it returns the type of identificator
void push_typeop(std::string lex);
void check_uno();
void eq_bool();
void eq_int();
void stack_clear();
void check_bin();

bool is_unary(std::string op);
bool is_assignment(std::string op);
bool is_comparison(std::string op);
bool is_binary(std::string op);
bool is_type(std::string op);
