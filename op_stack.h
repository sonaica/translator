#pragma once
#include <stack>
#include <string>

#include "lexical_analyzer.cpp"

enum STACK_ELEMENT_TYPE = {
    VARIABLE_TYPE,
    FUNCTION_NAME,
    OPERATOR,
    NONE
};

STACK_ELEMENT_TYPE get_basic_type(const std::string& str);

struct stack_element {
    STACK_ELEMENT_TYPE type;
    std::string type_info;
    std::string content;
    stack_element();
    stack_element(STACK_ELEMENT_TYPE type_, const std::string& type_info_, const std::string& content_);
};

// type (int, double, bool, utility)
// content (variable name, operator)
std::stack<stack_element> st;

void push_typeop(stack_element lex);
void check_uno();
void eq_bool();
void eq_int();
void stack_clear();
void stack_del();
void check_bin();

bool is_unary(stack_element op);
bool is_assignment(stack_element op);
bool is_comparison(stack_element op);
bool is_binary(stack_element op);
bool is_type(stack_element op);
bool is_member_access(stack_element op);
