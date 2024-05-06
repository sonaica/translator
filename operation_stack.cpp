#include "operation_stack.hpp"
#include "CompileError.cpp"
#include "RuntimeError.cpp"
#include "global_tids.hpp"
#include "poliz.cpp"

stack_element::stack_element() {}
stack_element::stack_element(STACK_ELEMENT_TYPE el_type_,
                             const std::string& type_info_,
                             const std::string& content_)
    : el_type(el_type_), type_info(type_info_), content(content_) {}

stack_element::stack_element(STACK_ELEMENT_TYPE el_type_, const std::string &type_info_, 
                             const std::string &content_, size_t poliz_pos_)
    : el_type(el_type_), type_info(type_info_),
      content(content_), poliz_pos(poliz_pos_) {}

void push_opstack(stack_element el, bool mode = true)
{
    if (mode) {
        if (el.el_type != OPERATOR_S) {
            el.poliz_pos = cur_ptr();
            push_stack({ELEMENT_TYPE::UNDEFINED, el.content});
        } else {
            push_stack({ELEMENT_TYPE::OPERATOR, el.type_info});
        }
    } else {
        el.poliz_pos = -1;
    }
    elems.push_back(el);
}

stack_element pop_opstack()
{
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

void identify_as_lvalue(stack_element &f)
{
    if (f.el_type != IDENTIFIER_S)
        return;
    f.el_type = LVALUE_S;
    f.type_info = IdTIDS.cur_tid()->check_id(f.content);
}

bool is_basic_type(const std::string &s)
{
    return s == "int" || s == "double" || s == "bool";
}

std::string get_array_type(const std::string &s)
{
    // type + _array
    return s.substr(0, s.size() - 6);
}

bool is_array(const std::string &s)
{
    return s.size() >= 6 && s.substr(s.size() - 6) == "_array";
}

void push_poliz_stack(const stack_element &s)
{
    switch (s.el_type) {
    case RVALUE_S:
        if (s.poliz_pos == -1)
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
        if (s.poliz_pos == -1)
            break;
        poliz[s.poliz_pos] = {ELEMENT_TYPE::VARIABLE_OPERAND, s.content};
        break;
    case FUNCTION_S:
        if (s.poliz_pos == -1)
            break;
        poliz[s.poliz_pos] = {ELEMENT_TYPE::FUNCTION_NAME, s.content};
        break;
    case OPERATOR_S:
        break;
    case IDENTIFIER_S:
        if (s.poliz_pos == -1)
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
    if (el.el_type != RVALUE_S || el.type_info != "bool")
        throw InvalidTypes();
}

void eq_int() {
    stack_element el = pop_opstack();
    if (el.el_type != RVALUE_S || el.type_info != "int")
        throw InvalidTypes();
}

bool is_assignment(stack_element op) {
    if (op.type_info == "=" || op.type_info == "<<=" || op.type_info == ">>=" || op.type_info == "+=" || op.type_info == "-=" ||
        op.type_info == "*=" || op.type_info == "**=" || op.type_info == "/=" || op.type_info == "//=" || op.type_info == "^=" ||
        op.type_info == "&=" || op.type_info == "|=" || op.type_info == "%=")
        return true;
    return false;
}

bool is_comparison(stack_element op) {
    if (op.type_info == "==" || op.type_info == "!=" || op.type_info == "<" || op.type_info == ">" || op.type_info == ">=" ||
        op.type_info == "<=")
        return true;
    return false;
}

bool is_logical(stack_element op) {
    if (op.type_info == "&" || op.type_info == "and" || op.type_info == "^" || op.type_info == "|" || op.type_info == "or" ||
        op.type_info == ">>" || op.type_info == "<<")
        return true;
    return false;
}

bool is_binary(stack_element op) {
    if (op.type_info == "*" || op.type_info == "/" || op.type_info == "%" || op.type_info == "+" || op.type_info == "-" ||
        op.type_info == "**")
        return true;
    return false;
}

bool is_unary(stack_element op) {
    if (op.type_info == "+" || op.type_info == "-" || op.type_info == "++" || op.type_info == "--" || op.type_info == "~")
        return true;
    return false;
}

bool is_type(stack_element a) {
    if (a.type_info == "int" || a.type_info == "bool" || a.type_info == "double" || StrTIDS.check_struct_existance(a.type_info)) return true;
    return false;
}

bool is_member_access(stack_element op) {
    return op.type_info == ".";
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
        push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, get_dominant_type(left, right), ""}, false);
    } else if (oper.type_info == "%") {
        if (get_dominant_type(left, right) == "double") {
            throw ModFromDoubleError();
        }
        push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, "int", ""}, false);
    } else if (oper.type_info == "<<" || oper.type_info == ">>" || oper.type_info == "&" || oper.type_info == "|"
                || oper.type_info == "^" || oper.type_info == "and" || oper.type_info == "or") {
        if (get_dominant_type(left, right) == "double")
            throw BitwiseOpsWithDoublesError();
        push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, "int", ""}, false);
    } else if (oper.type_info == "<" || oper.type_info == ">" || oper.type_info == "<=" || oper.type_info == ">="
                || oper.type_info == "==" || oper.type_info == "!=") {
        push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, "bool", ""}, false);
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
    if (oper.type_info == "++" || oper.type_info == "--") {
        if (arg.el_type != LVALUE_S) {
            throw CrementedRvalue();
        }
        if (arg.type_info != "int" && arg.type_info != "double") {
            throw InvalidCrementOperand();
        }
        push_opstack(arg, false);
    } else if (oper.type_info == "+u" || oper.type_info == "-u") {
        if (arg.type_info != "int" && arg.type_info != "double" && arg.type_info != "bool") {
            throw InvalidUnaryOperand();
        }
        push_opstack(arg, false);
    } else if (oper.type_info == "~") {
        if (arg.type_info != "int" && arg.type_info != "bool") {
            throw InvalidBinaryNotOperand();
        }
        push_opstack(arg, false);
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
    func.el_type = FUNCTION_S;
    std::reverse(args.begin(), args.end());
    func.type_info = FunTIDS.check_func_id(func.content);
    FunTIDS.check_param_count(func.content, args.size());
    for (size_t pos = 0; pos < args.size(); ++pos) {
        identify_as_lvalue(args[pos]);
        FunTIDS.check_func_par(func.content, pos, args[pos].type_info);
    }
    
    if (func.type_info != "void")
        push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, func.type_info, ""}, false);

    for (size_t pos = 0; pos < args.size(); ++pos) {
        push_poliz_stack(args[pos]);
    }
    push_poliz_stack(func);
}

void check_array_access()
{
    stack_element left, right;
    right = pop_opstack();
    left = pop_opstack();
    identify_as_lvalue(right);
    identify_as_lvalue(left);
    push_poliz_stack(left);
    push_poliz_stack(right);
    push_poliz({ELEMENT_TYPE::OPERATOR, "[]"});
    if (right.type_info != "int" && right.type_info != "bool") {
        throw InvalidArrayIndexation();
    }
    if (!is_array(left.type_info))
        throw InvalidArrayIndexation();
    push_opstack({STACK_ELEMENT_TYPE::LVALUE_S, get_array_type(left.type_info), left.content}, false);
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
    if (right.el_type == STACK_ELEMENT_TYPE::IDENTIFIER_S) {
        std::string tp = StrTIDS.check_id(left.type_info, right.content);
        push_opstack({STACK_ELEMENT_TYPE::LVALUE_S, tp, ""}, false);
    } else if (right.el_type == STACK_ELEMENT_TYPE::FUNCTION_S) {
        std::string tp = StrTIDS.check_func_id(left.type_info, right.content);
        push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, tp, ""}, false);
    } else {
        // smth strange
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

    if (oper.type_info == "=") {
        if (!is_basic_type(left.type_info) || !is_basic_type(right.type_info)) {
            throw AssignmentOfNonBasicTypes();
        }
        push_opstack(left, false);

        push_poliz_stack(left);
        push_poliz_stack(right);
        push_poliz_stack(oper);
    } else {
        std::string op = oper.type_info.substr(0, oper.type_info.size() - 1);
        push_opstack(left, false);
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, "=", ""});
        push_opstack(left, false);
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, op, ""});
        push_opstack(right, false);
        check_bin();
        check_assignment();
    }
}


