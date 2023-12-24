#include "stack.h"

bool is_assignment(std::string op) {
    if (op == "=" || op == "<<=" || op == ">>=" || op == "+=" || op == "-=" ||
        op == "*=" || op == "**=" || op == "/=" || op == "//=" || op == "^=" ||
        op == "&=" || op == "|=" || op == "%=")
        return true;
    return false;
}

bool is_comparison(std::string op) {
    if (op == "==" || op == "!=" || op == "<" || op == ">" || op == ">=" ||
        op == "<=")
        return true;
    return false;
}

bool is_logical(std::string op) {
    if (op == "&" || op == "and" || op == "^" || op == "|" || op == "or" ||
        op == ">>" || op == "<<")
        return true;
    return false;
}

bool is_binary(std::string op) {
    if (op == "*" || op == "/" || op == "%" || op == "+" || op == "-" ||
        op == "**")
        return true;
    return false;
}

bool is_unary(std::string op) {
    if (op == "+" || op == "-" || op == "++" || op == "--" || op == "~")
        return true;
    return false;
}

bool is_type(std::string a) {
    if (a == "int" || a == "bool" || a == "double") return true;
    return false;
}

void push_typeop(std::string lex) {
    st.push(lex);
    return;
}

void check_bin() {
    std::string a = st.top();
    st.pop();
    std::string op = st.top();
    st.pop();
    std::string b = st.top();
    st.pop();
    if (!is_type(a) || !is_type(b)) throw InvalidTypes();
    if (is_assignment(op)) {
        swap(a, b);
        st.push(a);
        return;
    }
    if (is_binary(op)) {
        if (a == b)
            st.push(a);
        else if (a == "int" && b == "double" || a == "double" && b == "int")
            st.push("double");
        else if (a == "bool" && b == "int" || a == "int" && b == "bool")
            st.push("int");
        else if (a == "double" && b == "bool" || a == "bool" && b == "double")
            st.push("double");
        return;
    }
    if (is_logical(op)) {
        if (a == "double" || b == "double") throw InvalidTypes();
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
    std::string a = st.top();
    st.pop();
    std::string op = st.top();
    st.pop();
    if (op == "!" && is_type(a)) {
        st.push("bool");
        return;
    }
    if (is_unary(op) && a == "int") {
        st.push("int");
        return;
    }
    if (is_unary(op) && op != "~" && a == "double") {
        st.push("double");
        return;
    }
    if (is_unary(op) && op != "++" && op != "--" && a == "bool") {
        st.push("bool");
        return;
    }
    throw InvalidTypes();
}

void eq_bool() {
    std::string a = st.top();
    st.pop();
    if (a != "bool") throw InvalidTypes();
}

void eq_int() {
    std::string a = st.top();
    st.pop();
    if (a != "int") throw InvalidTypes();
}

void stack_clear() {
    while (!st.empty()) {
        st.pop();
    }
    return;
}