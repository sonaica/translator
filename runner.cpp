#pragma once
#include "syntax_analyzer.cpp"
#include <vector>
#include <functional>
#include <cmath>
#include <cstdint>
#include "RuntimeError.cpp"
#include "global_tids.hpp"
#include "uni_ptr.cpp"

/*
vector<str> st;

struct str {
    void* mem_adr = nullptr;
    map<string, int> mp;
    void* get(string str) {
        string tp = get_type(str);

        return (mem_adr + mp[str]);
    }
}

// start..start + len;
*/
/*
a1.a2.a3.a4

a1

. - p1 p2 -> p1.p2
(p1)`p2.

sz
mem; mem + sz -> mem + pos; mem + pos + len(a2)

pos
*/

struct value {
    std::string tp;
    std::string utillity;
    bool is_var_ = false;
    uni_ptr ptr;

    bool is_var() const {
        return is_var_;
    }

    value() {
        tp = "undefined";
    }

    value(const std::string& tmp) {
        utillity = tmp;
        tp = "util";
    }

    template<class T>
    value(const std::string& typ, T val) {
        tp = typ;
        ptr.create();
        if (tp == "int") ptr.get_int() = val;
        if (tp == "double") ptr.get_double() = val;
        if (tp == "bool") ptr.get_bool() = val;
    }

    value(int64_t val) {
        ptr.create();
        ptr.get_int() = val;
        tp = "int";
    }

    value(double val) {
        ptr.create();
        ptr.get_double() = val;
        tp = "double";
    }

    value(bool val) {
        ptr.create();
        ptr.get_bool() = val;
        tp = "bool";
    }

    ~value() {}

    int64_t get_int_val() const {
        return *ptr.int_ptr();
    }
    int64_t& get_int_ref() {
        return ptr.get_int();
    }
    int64_t* get_int_ptr() {
        return ptr.int_ptr();
    }
    
    double get_double_val() const {
        return *ptr.double_ptr();
    }
    double& get_double_ref() {
        return ptr.get_double();
    }
    double* get_double_ptr() {
        return ptr.double_ptr();
    }

    bool get_bool_val() const {
        return *ptr.bool_ptr();
    }
    bool& get_bool_ref() {
        return ptr.get_bool();
    }
    bool* get_bool_ptr() {
        return ptr.bool_ptr();
    }

    template<class T>
    T cast_to() const {
        if (tp == "bool") return get_bool_val();
        if (tp == "int") return get_int_val();
        if (tp == "double") return get_double_val();
        return 0;
    }

    value& assign(const value& other) {
        if (!is_var())
            throw AssignmentToRvalueError();
        if (tp == "int") get_int_ref() = other.cast_to<int>();
        if (tp == "double") get_double_ref() = other.cast_to<double>();
        if (tp == "bool") get_bool_ref() = other.cast_to<bool>();
        return *this;
    }
};

void make_rvalue(value& val) {
    val.is_var_ = false;
}

string get_dom_type(const value& a, const value& b) {
    if (a.tp == "double" || b.tp == "double") return "double";
    return "int";
}

value cast_to_type(value val, std::string tp) {
    make_rvalue(val);
    val.tp = tp;
    return val;
}

std::vector<value> value_stack;
std::vector<value> main_match_terms;
std::vector<std::vector<value>> match_case_terms;

std::map<std::string, std::function<value(value, value)>> binary_rvalue_operations = {
    {"+", [](value a, value b) -> value { return value(get_dom_type(a, b), (get_dom_type(a, b) == "double") 
        ? a.get_double_val() + b.get_double_val() : a.get_int_val() + b.get_int_val()); }},
    {"-", [](value a, value b) -> value { return value(get_dom_type(a, b), (get_dom_type(a, b) == "double")
        ? a.get_double_val() - b.get_double_val() : a.get_int_val() - b.get_int_val()); }},
    {"*", [](value a, value b) -> value { return value(get_dom_type(a, b), (get_dom_type(a, b) == "double")
        ? a.get_double_val() * b.get_double_val() : a.get_int_val() * b.get_int_val()); }},
    {"/", [](value a, value b) -> value {
        std::string dom_type = get_dom_type(a, b);
        if (dom_type == "double") {
            return value(dom_type, a.get_double_val() / b.get_double_val());
        } else if (b.get_int_val() == 0) {
            throw DivisionByZeroError();
        }
        return value(dom_type, a.get_int_val() / b.get_int_val());
    }},
    {"%", [](value a, value b) -> value {
        std::string dom_type = get_dom_type(a, b);
        if (dom_type == "double")
            throw ModFromDoubleError();
        if (b.get_int_val() == 0)
            throw DivisionByZeroError();
        return value(dom_type, a.get_int_val() % b.get_int_val());
    }},
    {"**", [](value a, value b) -> value {
        std::string dom_type = get_dom_type(a, b);
        if (dom_type == "double" || b.get_int_val() < 0 || 
            (((a.tp == "int") ? a.get_int_val() : a.get_double_val()) <= 0) && b.tp == "double") {
            return value("double", pow(a.cast_to<double>(), b.cast_to<double>()));
        } else if (a.get_int_val() == 0 && b.get_int_val() <= 0) {
            throw ZeroRaisedToOrbitError();
        }
        int64_t base, pw, res = 1;
        base = a.get_int_val();
        pw = b.get_int_val();
        while (pw > 0) {
            if (pw & 1)
                res *= base;
            base *= base;
            pw >>= 1;
        }
        return value("int", res);
    }},
    {"<<", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.get_int_val() << b.get_int_val());
    }},
    {">>", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.get_int_val() >> b.get_int_val());
    }},
    {"&", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.get_int_val() & b.get_int_val());
    }},
    {"|", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.get_int_val() | b.get_int_val());
    }},
    {"^", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.get_int_val() ^ b.get_int_val());
    }},
    {"and", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.get_int_val() & b.get_int_val());
    }},
    {"or", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.get_int_val() | b.get_int_val());
    }},
    {"<", [](value a, value b) -> value { return value("bool", (get_dom_type(a, b) == "double") ?
        a.get_double_val() < b.get_double_val() : a.get_int_val() < b.get_int_val());} },
    {"<=", [](value a, value b) -> value { return value("bool", (get_dom_type(a, b) == "double") ? 
        a.get_double_val() <= b.get_double_val() : a.get_int_val() <= b.get_int_val());} },
    {">", [](value a, value b) -> value {return value("bool", (get_dom_type(a, b) == "double") ?
        a.get_double_val() > b.get_double_val() : a.get_int_val() > b.get_int_val());} },
    {">=", [](value a, value b) -> value {return value("bool", (get_dom_type(a, b) == "double") ?
        a.get_double_val() >= b.get_double_val() : a.get_int_val() >= b.get_int_val());} },
    {"==", [](value a, value b) -> value {return value("bool", (get_dom_type(a, b) == "double") ?
        a.get_double_val() == b.get_double_val() : a.get_int_val() == b.get_int_val());} },
    {"!=", [](value a, value b) -> value {return value("bool", (get_dom_type(a, b) == "double") ?
        a.get_double_val() != b.get_double_val() : a.get_int_val() != b.get_int_val());} }
};

void pop_value_stack_to(value& a) {
    a = value_stack.back();
    value_stack.pop_back();
}

template<class T>
T pop_from_stack(const std::string& tp) {
    value val = value_stack.back();
    T result;
    if (tp == "int")
        result = val.get_int_val();
    else if (tp == "double")
        result = val.get_double_val();
    else if (tp == "bool")
        result = val.get_bool_val();
    value_stack.pop_back();
    return result;
}

void output_poliz_element(const poliz_element& element) {
    std::cout << element_type_translation[element.type_info] << " -> ";
    if (element.type_info == ELEMENT_TYPE::POSITION) {
        std::cout << element.position << "\n";
    } else {
        std::cout << element.maker << "\n";
    }
}

void InitializeGlobalVariables() {
    // Пока что глобал переменных нет
}

std::vector<char> special_symbols = {
    '\"', '\\', 'n'
};

// Пробегает через полиз пока не уткнётся в return/end_of_function/конец полиза
void Run(std::size_t run_from, const std::string& function_name) {
    if (function_name != "main") {
        // add formal parameters;
    }
    std::size_t cur_pos = run_from;
    while (cur_pos < poliz.size()) {
        poliz_element cur_element = poliz[cur_pos++];
        // std::cout << cur_pos - 1 << ": ";
        // output_poliz_element(cur_element);
        // continue;
        // std::cout << "!";
        switch (cur_element.type_info) {
            case LITERAL_OPERAND: {
                
                auto from = find(cur_element.maker.begin(), cur_element.maker.end(), '_');
                int literal_tp = std::stoi(cur_element.maker.substr(0, from - cur_element.maker.begin()));
                switch (literal_tp) {
                    case literal_int_type: {
                        int64_t result = std::stoll(cur_element.maker.substr(from - cur_element.maker.begin() + 1));
                        value_stack.emplace_back("int", result);
                        break;
                    }
                    case literal_double_type: {
                        double result = std::stod(cur_element.maker.substr(from - cur_element.maker.begin() + 1));
                        value_stack.emplace_back("double", result);
                        break;
                    }
                    case literal_bool_type: {
                        bool result = (cur_element.maker.substr(from - cur_element.maker.begin() + 1) == "true") ? true : false;
                        value_stack.emplace_back("bool", result);
                        break;
                    }
                    case string_literal_type: {
                        cur_element.maker = cur_element.maker.substr(from - cur_element.maker.begin() + 1);
                        cur_element.maker = cur_element.maker.substr(1, (int)cur_element.maker.size() - 2);
                        value_stack.emplace_back(cur_element.maker);
                        break;
                    }
                }
                
                break;
            }
            case OPERAND: {
                value_stack.emplace_back(cur_element.maker);
                break;
            }
            case OPERATOR: {
                if (cur_element.maker == "++" || cur_element.maker == "--") {
                    int delta = 1;
                    if (cur_element.maker == "--")
                        delta = -1;
                    value val;
                    pop_value_stack_to(val);
                    if (val.tp == "int") {
                        if (val.is_var())
                            val.get_int_ref() += delta;
                    } else if (val.tp == "double") {
                        if (val.is_var())
                            val.get_double_ref() += delta;
                    } else {
                        throw CrementOnBoolError();
                    }
                    value_stack.push_back(val);
                } else if (cur_element.maker == "-u") {
                    value val;
                    pop_value_stack_to(val);
                    if (val.tp == "int") {
                        val.get_int_ref() *= -1;
                    } else if (val.tp == "double") {
                        val.get_double_ref() *= -1;
                    } else if (val.tp == "bool") {
                        val.get_bool_ref() *= -1;
                    }
                    value_stack.push_back(val);
                } else if (cur_element.maker == "+u"){
                    break;  
                } else if (cur_element.maker == "~") {
                    value val;
                    pop_value_stack_to(val);
                    if (val.tp == "int") {
                        val.get_int_ref() = ~val.get_int_ref();
                    } else if (val.tp == "double") {
                        // stop right here
                    } else if (val.tp == "bool") {
                        val.get_bool_ref() = ~val.get_bool_ref();
                    }
                    value_stack.push_back(val);
                } else if (cur_element.maker == "()") {
                    // TO DO
                } else if (cur_element.maker == "[]") {
                    // WORKS FOR BASIC TYPES ONLY!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    value ind_elem, arr;
                    pop_value_stack_to(ind_elem);
                    pop_value_stack_to(arr);
                    string needed_type = arr.tp.substr(0, find(arr.tp.begin(), arr.tp.end(), '_') - arr.tp.begin());
                    value result;
                    if (needed_type == "int") {
                        result = *(arr.get_int_ptr() + ind_elem.get_int_ref());
                    } else if (needed_type == "double") {
                        result = *(arr.get_double_ptr() + ind_elem.get_int_ref());
                    } else if (needed_type == "bool") {
                        result = *(arr.get_bool_ptr()+ ind_elem.get_int_ref());
                    }
                    value_stack.push_back(result);
                } else if (cur_element.maker == ".") {
                    // TO DO
                } else if (cur_element.maker == "input") {
                    size_t count_operands = pop_from_stack<int64_t>("int");
                    std::vector<value> vals(count_operands);
                    bool ok = true;
                    for (int ind = count_operands - 1; ind >= 0; --ind) {
                        pop_value_stack_to(vals[ind]);
                    }
                    for (value& v : vals) {
                        if (v.tp == "int") {
                            ok &= bool(std::cin >> v.get_int_ref());
                        } else if (v.tp == "bool") {
                            ok &= bool(std::cin >> v.get_bool_ref());
                        } else if (v.tp == "double") {
                            ok &= bool(std::cin >> v.get_double_ref());
                        } else {
                            ok = false;
                        }
                    }
                    value_stack.push_back(value("bool", ok));
                } else if (cur_element.maker == "output") {
                    size_t count_operands = pop_from_stack<int64_t>("int");
                    std::vector<value> vals(count_operands);
                    
                    for (int ind = count_operands - 1; ind >= 0; --ind) {
                        pop_value_stack_to(vals[ind]);
                    }
                    for (const value& v : vals) {
                        if (v.tp == "int") {
                            std::cout << v.get_int_val();
                        } else if (v.tp == "bool") {
                            std::cout << v.get_bool_val();
                        } else if (v.tp == "double") {
                            std::cout << v.get_double_val();
                        } else if (v.tp == "util") {
                            bool note_special = false;
                            for (char c : v.utillity) {
                                if (c == '\\') {
                                    note_special = true;
                                } else {
                                    if (note_special && 
                                    find(special_symbols.begin(), special_symbols.end(), c) != special_symbols.end()) {
                                        if (c == 'n') {
                                            std::cout << '\n';
                                        } else if (c == '\\') {
                                            std::cout << '\\';
                                        } else if (c == '\"') {
                                            std::cout << '\"';
                                        }
                                    } else {
                                        std::cout << c;
                                    }
                                    note_special = false;
                                }
                            }
                            if (note_special) {
                                std::cout << '\\';
                            }
                        }
                    }
                } else if (cur_element.maker == ",") {
                    // doesn't happen?
                } else {
                    value arg1, arg2;
                    pop_value_stack_to(arg2);
                    pop_value_stack_to(arg1);
                    if (binary_rvalue_operations.count(cur_element.maker) == 0) {
                        if (cur_element.maker == "=") {
                            value_stack.push_back(arg1.assign(arg2));
                        } else {
                            std::string op = cur_element.maker.substr(0, cur_element.maker.size() - 1);
                            value_stack.push_back(arg1.assign(binary_rvalue_operations[op](arg1, arg2)));
                        }
                    } else {
                        value_stack.push_back(binary_rvalue_operations[cur_element.maker](arg1, arg2));
                    }
                }
                break;
            }
            case UNCOND_MOVE: {
                cur_pos = value_stack.back().get_int_ref();
                value_stack.pop_back();
                break; 
            }
            case MOVE_BY_FALSE: {
                int move_to = value_stack.back().get_int_ref();
                value_stack.pop_back();
                bool res = value_stack.back().cast_to<bool>();
                value_stack.pop_back();
                if (!res)
                    cur_pos = move_to;
                break;
            }
            case MOVE_BY_TRUE: {
                int move_to = value_stack.back().get_int_ref();
                value_stack.pop_back();
                bool res = value_stack.back().cast_to<bool>();
                value_stack.pop_back();
                if (res)
                    cur_pos = move_to;
                break;
            }
            case UNDEFINED: {
                break;
            }
            case FUNCTION_NAME: {
                // to do
                break;
            }
            case POSITION: {
                value_stack.emplace_back(value("int", cur_element.position));
                break;
            }
            case SEPARATOR: {
                // TO DO???
                break;
            }
            case REM_MATCH_MAIN_TERM: {
                main_match_terms.push_back(value_stack.back());
                match_case_terms.push_back(std::vector<value>());
                value_stack.pop_back();
                break;
            }
            case REM_MATCH_CASE: {
                match_case_terms.back().push_back(value_stack.back());
                value_stack.pop_back();
                break;
            }
            case GET_MATCH_MAIN_TERM: {
                value_stack.push_back(main_match_terms.back());
                break;
            }
            case GET_CASE_TERM: {
                value_stack.push_back(match_case_terms.back()[stoi(cur_element.maker)]);
                if (int(match_case_terms.size()) - 1 == stoi(cur_element.maker)) {
                    match_case_terms.pop_back();
                    main_match_terms.pop_back();
                }
                break;
            }
            case MAKE_VARIABLE: {
                value tp, name;
                pop_value_stack_to(name);
                pop_value_stack_to(tp);
                IdTIDS.cur_tid()->push_id(Value(tp.utillity, name.utillity));
                break;
            }
            case MAKE_ARRAY_AUTO: {
                // TO DO
                break;
            }
            case MAKE_ARRAY_EXACT: {
                // TO DO
                break;
            }
            case RETURN_OPERATOR: {
                // TO DO
                break;   
            }
            case END_OF_FUNCTION: {
                // TO DO
                break;
            }
            case VARIABLE_OPERAND: {
                value to_add;
                to_add.ptr.ptr = IdTIDS.cur_tid()->find_id(cur_element.maker);
                to_add.ptr.deleteable = false;
                to_add.tp = IdTIDS.cur_tid()->check_id(cur_element.maker);
                to_add.is_var_ = true;
                value_stack.push_back(to_add);
                
                break;
            }
            case CREATE_TID: {
                IdTIDS.create_TID();
                break;
            }
            case DELETE_TID: {
                IdTIDS.del_TID();
                break;
            }
        }
    }
}