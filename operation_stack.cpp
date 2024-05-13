#include "operation_stack.hpp"
#include "CompileError.cpp"
#include "RuntimeError.cpp"
#include "global_tids.hpp"
#include "poliz.cpp"
#include <cassert>

stack_element::stack_element() {}
stack_element::stack_element(STACK_ELEMENT_TYPE el_type_,
                             const std::string& type_info_,
                             const std::string& content_,
                             bool in_poliz_)
    : el_type(el_type_), type_info(type_info_), content(content_), in_poliz(in_poliz_) {}

stack_element::stack_element(STACK_ELEMENT_TYPE el_type_, 
                             const std::string &type_info_, 
                             const std::string &content_, 
                             size_t poliz_pos_, 
                             bool in_poliz_)
    : el_type(el_type_), type_info(type_info_),
      content(content_), poliz_pos(poliz_pos_), in_poliz(in_poliz_) {}

void push_opstack(stack_element el)
{
    if (el.in_poliz && el.el_type != INPUT_END_S && el.el_type != OUTPUT_END_S) {
        if (el.el_type != OPERATOR_S) {
            el.poliz_pos = cur_ptr();
            push_poliz({ELEMENT_TYPE::UNDEFINED, el.content});
        }
    } else {
        el.poliz_pos = -1;
    }
    elems.push_back(el);
}

stack_element pop_opstack()
{
    assert(!elems.empty());
    stack_element st = elems.back();
    elems.pop_back();
    return st;
}


std::string get_dominant_type(stack_element &f, stack_element &s)
{
    if (f.type_info == "double" || s.type_info == "double")
        return "double";
    return "int";
}

void identify_as_lvalue(stack_element & f, std::string str)
{
    if (f.el_type != IDENTIFIER_S)
        return;
    f.el_type = LVALUE_S;
    if (str == "") {
        f.type_info = IdTIDS.cur_tid()->check_id(f.content);
    } else {
        f.type_info = StrTIDS.check_id(str, f.content);
    }
}

void identify_as_function(stack_element &f)
{
    if (f.el_type != IDENTIFIER_S)
        return;
    f.el_type = FUNCTION_S;
    f.func_ptr = FunTIDS.getFunction(f.content);
}

void push_poliz_stack(const stack_element &s)
{
    switch (s.el_type) {
    case RVALUE_S:
        if (!s.in_poliz)
            break;
        if (s.content.empty())
            break;
        if (s.type_info == "bool")
            poliz[s.poliz_pos] = {ELEMENT_TYPE::LITERAL_OPERAND, std::to_string(literal_bool_type) + "_" + s.content};
        else if (s.type_info == "int")
            poliz[s.poliz_pos] = {ELEMENT_TYPE::LITERAL_OPERAND, std::to_string(literal_int_type) + "_" + s.content};
        else
            poliz[s.poliz_pos] = {ELEMENT_TYPE::LITERAL_OPERAND, std::to_string(literal_double_type) + "_" + s.content};
        break;
    case LVALUE_S:
        if (!s.in_poliz)
            break;
        poliz[s.poliz_pos] = {ELEMENT_TYPE::VARIABLE_OPERAND, s.content};
        break;
    case FUNCTION_S:
        if (!s.in_poliz)
            break;
        poliz[s.poliz_pos] = {ELEMENT_TYPE::FUNCTION_NAME, s.content};
        break;
    case OPERATOR_S:
        if (s.in_poliz)
            push_poliz(poliz_element(ELEMENT_TYPE::OPERATOR, s.type_info));
        break;
    case IDENTIFIER_S:
        if (!s.in_poliz)
            break;
        poliz[s.poliz_pos] = {ELEMENT_TYPE::STRUCT_MEMBER, s.content};
        break; 
    }
}

void push_remaining_elements_to_stack()
{
    for (stack_element st : elems) {
        identify_as_lvalue(st);
        push_poliz_stack(st);
    }
    stack_clear();
}

void stack_clear() {
    while (!elems.empty()) {
        elems.pop_back();
    }
}

void eq_bool() {
    stack_element el = pop_opstack();
    identify_as_lvalue(el);
    if (!is_basic_type(el.type_info)) {
        throw InvalidTypes();
    }
}

void eq_int() {
    stack_element el = pop_opstack();
    identify_as_lvalue(el);
    if (el.type_info != "int")
        throw InvalidTypes();
}

void check_bin() {
    stack_element left, right, oper;
    right = pop_opstack();
    oper = pop_opstack();
    left = pop_opstack();
    identify_as_lvalue(left);
    identify_as_lvalue(right);
    push_poliz_stack(left);
    push_poliz_stack(right);
    push_poliz_stack(oper);
    if (oper.type_info == "+" || oper.type_info == "-" || oper.type_info == "*" || oper.type_info == "/") {
        push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, get_dominant_type(left, right), "", false});
    } else if (oper.type_info == "%") {
        if (get_dominant_type(left, right) == "double") {
            throw ModFromDoubleError();
        }
        push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, "int", "", false});
    } else if (oper.type_info == "<<" || oper.type_info == ">>" || oper.type_info == "&" || oper.type_info == "|"
                || oper.type_info == "^" || oper.type_info == "and" || oper.type_info == "or") {
        if (get_dominant_type(left, right) == "double")
            throw BitwiseOpsWithDoublesError();
        push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, "int", "", false});
    } else if (oper.type_info == "<" || oper.type_info == ">" || oper.type_info == "<=" || oper.type_info == ">="
                || oper.type_info == "==" || oper.type_info == "!=") {
        if (!is_basic_type(left.type_info) || !is_basic_type(right.type_info))
            throw InvalidComparison();
        push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, "bool", "", false});
    } else {
        // UNDEFINED OPERATION???
    }

}

void check_uno() {
    stack_element arg, oper;
    arg = pop_opstack();
    oper = pop_opstack();
    identify_as_lvalue(arg);
    push_poliz_stack(arg);
    push_poliz_stack(oper);
    arg.poliz_pos = false;
    arg.in_poliz = false;
    if (oper.type_info == "++" || oper.type_info == "--") {
        if (arg.el_type != LVALUE_S) {
            throw CrementedRvalue();
        }
        if (arg.type_info != "int" && arg.type_info != "double") {
            throw InvalidCrementOperand();
        }
        
        push_opstack(arg);
    } else if (oper.type_info == "+u" || oper.type_info == "-u") {
        if (arg.type_info != "int" && arg.type_info != "double" && arg.type_info != "bool") {
            throw InvalidUnaryOperand();
        }
        push_opstack(arg);
    } else if (oper.type_info == "~") {
        if (arg.type_info != "int" && arg.type_info != "bool") {
            throw InvalidBinaryNotOperand();
        }
        push_opstack(arg);
    } else {
        // UNDEFIND UNARY OPERATION
    }
}

void check_function_call()
{
    stack_element func;
    std::vector<stack_element> args;
    for (stack_element cur = pop_opstack(); cur.el_type != STACK_ELEMENT_TYPE::OPERATOR_S; cur = pop_opstack()) {
        args.push_back(cur);
    }
    func = pop_opstack();
    identify_as_function(func);
    std::reverse(args.begin(), args.end());
    func.type_info = func.func_ptr->return_type();
    func.func_ptr->check_param_count(args.size());
    for (size_t pos = 0; pos < args.size(); ++pos) {
        identify_as_lvalue(args[pos]);
        func.func_ptr->check_func_par(pos, args[pos].type_info);
    }
    if (func.type_info != "void")
    push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, func.type_info, "", false});

    for (size_t pos = 0; pos < args.size(); ++pos) {
        push_poliz_stack(args[pos]);
    }
    push_poliz_stack(func);
    push_poliz(poliz_element(ELEMENT_TYPE::OPERATOR, "()"));

}

void check_array_access()
{
    stack_element left, oper, right;
    right = pop_opstack();
    oper = pop_opstack();
    left = pop_opstack();
    identify_as_lvalue(right);
    identify_as_lvalue(left);
    push_poliz_stack(left);
    push_poliz_stack(right);
    push_poliz_stack(oper);
    if (right.type_info != "int" && right.type_info != "bool") {
        throw InvalidArrayIndexation();
    }
    if (!is_array(left.type_info))
        throw InvalidArrayIndexation();
    push_opstack({STACK_ELEMENT_TYPE::LVALUE_S, get_array_type(left.type_info), left.content, false});
}

void check_member_access()
{
    stack_element left, oper, right;
    right = pop_opstack();
    oper = pop_opstack();
    left = pop_opstack();
    identify_as_lvalue(left);
    push_poliz_stack(left);
    push_poliz_stack(right);
    push_poliz_stack(oper);
    if (StrTIDS.check_id_safe(left.type_info, right.content)) {
        push_opstack({STACK_ELEMENT_TYPE::LVALUE_S, StrTIDS.check_id(left.type_info, right.content), "", false});
    } else if (StrTIDS.check_func_id_safe(left.type_info, right.content)) {
        stack_element el(FUNCTION_S, "", "", false);
        el.func_ptr = StrTIDS.getFunction(left.type_info, right.content);
        push_opstack(el);
    }
}

void check_assignment()
{
    stack_element left, right, oper;
    right = pop_opstack();
    oper = pop_opstack();
    left = pop_opstack();
    identify_as_lvalue(left);
    identify_as_lvalue(right);
    if (left.el_type == RVALUE_S) {
        throw AssignmentToRvalueError();
    }

    push_poliz_stack(left);
    push_poliz_stack(right);
    push_poliz_stack(oper);
    
    left.in_poliz = right.in_poliz = oper.in_poliz = false;
    if (oper.type_info == "=") {
        if (!is_basic_type(left.type_info) || !is_basic_type(right.type_info)) {
            throw AssignmentOfNonBasicTypes();
        }
        push_opstack(left);
    } else {
        std::string op = oper.type_info.substr(0, oper.type_info.size() - 1);
        push_opstack(left);
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, "=", "", false});
        push_opstack(left);
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, op, "", false});
        push_opstack(right);
        check_bin();
        check_assignment();
    }
}

void check_input()
{
    size_t cnt_ops = 0;
    for (stack_element cur = pop_opstack(); cur.el_type != INPUT_END_S; cur = pop_opstack()) {
        identify_as_lvalue(cur);
        ++cnt_ops;
        push_poliz_stack(cur);
        if (cur.el_type != LVALUE_S) {
            throw InputNonLvalueObjectError();
        }
    }
    push_poliz(poliz_element(LITERAL_OPERAND, std::to_string(literal_int_type) + "_" + 
                                                std::to_string(cnt_ops)));
    push_poliz(poliz_element(ELEMENT_TYPE::OPERATOR, "input"));
}

void check_left()
{
    for (stack_element& cur : elems) {
        identify_as_lvalue(cur);
        push_poliz_stack(cur);
    }
}
