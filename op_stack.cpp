#pragma once
#include "op_stack.h"
#include "CompileError.cpp"
#include "op_stack.hpp"

bool is_assignment(stack_element op) {
    if (op.content == "=" || op.content == "<<=" || op.content == ">>=" || op.content == "+=" || op.content == "-=" ||
        op.content == "*=" || op.content == "**=" || op.content == "/=" || op.content == "//=" || op.content == "^=" ||
        op.content == "&=" || op.content == "|=" || op.content == "%=")
        return true;
    return false;
}

bool is_comparison(stack_element op) {
    if (op.content == "==" || op.content == "!=" || op.content == "<" || op.content == ">" || op.content == ">=" ||
        op.content == "<=")
        return true;
    return false;
}

bool is_logical(stack_element op) {
    if (op.content == "&" || op.content == "and" || op.content == "^" || op.content == "|" || op.content == "or" ||
        op.content == ">>" || op.content == "<<")
        return true;
    return false;
}

bool is_binary(stack_element op) {
    if (op.content == "*" || op.content == "/" || op.content == "%" || op.content == "+" || op.content == "-" ||
        op.content == "**")
        return true;
    return false;
}

bool is_unary(stack_element op) {
    if (op.content == "+" || op.content == "-" || op.content == "++" || op.content == "--" || op.content == "~")
        return true;
    return false;
}

bool is_type(stack_element a) {
    if (a.type == INT || a.type == BOOL || a.type == DOUBLE || StrTIDS.check_struct_existance(a.type)) return true;
    return false;
}

bool is_member_access(stack_element op) {
    return op.content == ".";
}

STACK_ELEMENT_TYPE get_basic_type(const std::string &str)
{
    if (str == "int") return STACK_ELEMENT_TYPE::INT;
    if (str == "double") return STACK_ELEMENT_TYPE::DOUBLE;
    if (str == "bool") return STACK_ELEMENT_TYPE::BOOL;
    return STACK_ELEMENT_TYPE::NONE;
}

void push_typeop(stack_element lex)
{
    st.push(lex);
    return;
}

void check_bin() {
    stack_element a = st.top();
    st.pop();
    stack_element op = st.top();
    st.pop();
    stack_element b = st.top();
    st.pop();
    if (!is_type(a) || !is_type(b)) throw InvalidTypes();
    if (is_member_access(op)) {
        return;
    }
    if (is_assignment(op)) {
        swap(a, b);
        st.push(a);
        return;
    }
    if (is_binary(op)) {
        if (a.type == b.type)
            st.push(a);
        else if (a.type == "int" && b.type == "double" || a.type == "double" && b.type == "int")
            st.push("double");
        else if (a.type == "bool" && b.type == "int" || a.type == "int" && b.type == "bool")
            st.push("int");
        else if (a.type == "double" && b.type == "bool" || a.type == "bool" && b.type == "double")
            st.push("double");
        return;
    }
    if (is_logical(op)) {
        if (a.type == "double" || b.type == "double") throw InvalidTypes();
        st.push("int");
        return;
    }
    if (is_comparison(op)) {
        st.push("bool");
        return;
    }
    throw InvalidTypes();
    return;
}

void check_uno() {
    stack_element a = st.top();
    st.pop();
    stack_element op = st.top();
    st.pop();
    if (op.content == "!" && is_type(a)) {
        st.push("bool");
        return;
    }
    if (is_unary(op) && a != "bool" && a.content) {
        st.push("int");
        return;
    }
    if (is_unary(op) && op != "~" && a == "double") {
        st.push("double");
        return;
    }
    if (is_unary(op) && op.content != "++" && op.content != "--" && a == "bool") {
        st.push("bool");
        return;
    }
    throw InvalidTypes();
}


void eq_bool() {
    stack_element a = st.top();
    st.pop();
    if (!is_type(a)) throw InvalidTypes();
}

void eq_int() {
    stack_element a = st.top();
    st.pop();
    if (a.type != "int") throw InvalidTypes();
}

void stack_clear() {
    while (!st.empty()) {
        st.pop();
    }
    return;
}

void stack_del(){
    st.pop();
    return;
}

stack_element::stack_element() {}

stack_element::stack_element(STACK_ELEMENT_TYPE type_, const std::string &type_info_, const std::string &content_)
    : type(type_), type_info(type_info_), content(content_) {}