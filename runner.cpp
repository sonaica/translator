#pragma once
#include "syntax_analyzer.cpp"
#include <vector>
#include <functional>
#include <cmath>
#include <cstdint>
#include "RuntimeError.cpp"
#include "global_tids.hpp"
#include "uni_ptr.cpp"

struct func_info {
    Function* func_ptr = nullptr;
    std::string str_tp = NOT_A_TYPE;
    cool_byte* str_mem = nullptr;
};

struct value {
    std::string tp;
    std::string name;
    bool is_value_ = true;
    bool is_var_ = false;
    uni_ptr ptr;
    func_info fun_info;

    bool is_var() const {
        return is_var_;
    }

    bool is_value() const {
        return is_value_;
    }

    value() {
        tp = "undefined";
    }

    ~value() {}

    value(const std::string& tmp) {
        name = tmp;
        is_value_ = false;
        tp = "util";
    }

    template<class T>
    value(const std::string& typ, T val) {
        tp = typ;
        ptr.create(get_type_size(typ, StrTIDS));
        if (tp == "int") ptr.get_int() = val;
        if (tp == "double") ptr.get_double() = val;
        if (tp == "bool") ptr.get_bool() = val;
    }

    value(int64_t val) {
        ptr.create(get_type_size("int", StrTIDS));
        ptr.get_int() = val;
        tp = "int";
    }

    value(double val) {
        ptr.create(get_type_size("double", StrTIDS));
        ptr.get_double() = val;
        tp = "double";
    }

    value(bool val) {
        ptr.create(get_type_size("bool", StrTIDS));
        ptr.get_bool() = val;
        tp = "bool";
    }

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

    void copy(const value& other) {
        if (ptr.is_null()) {
            ptr.create(get_type_size(tp, StrTIDS));
        }
        if (tp == "int") get_int_ref() = other.cast_to<int>();
        if (tp == "double") get_double_ref() = other.cast_to<double>();
        if (tp == "bool") get_bool_ref() = other.cast_to<bool>();
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

string get_dom_type(const value& a, const value& b) {
    if (a.tp == "double" || b.tp == "double") return "double";
    return "int";
}

std::vector<std::vector<value>> value_stack;
std::vector<value> main_match_terms;

std::map<std::string, std::function<value(value, value)>> binary_rvalue_operations = {
    {"+", [](value a, value b) -> value { return value(get_dom_type(a, b), (get_dom_type(a, b) == "double") 
        ? a.cast_to<double>() + b.cast_to<double>() : a.cast_to<int>() + b.cast_to<int>()); }},
    {"-", [](value a, value b) -> value { return value(get_dom_type(a, b), (get_dom_type(a, b) == "double")
        ? a.cast_to<double>() - b.cast_to<double>() : a.cast_to<int>() - b.cast_to<int>()); }},
    {"*", [](value a, value b) -> value { return value(get_dom_type(a, b), (get_dom_type(a, b) == "double")
        ? a.cast_to<double>() * b.cast_to<double>() : a.cast_to<int>() * b.cast_to<int>()); }},
    {"/", [](value a, value b) -> value {
        std::string dom_type = get_dom_type(a, b);
        if (dom_type == "double") {
            return value(dom_type, a.cast_to<double>() / b.cast_to<double>());
        } else if (b.cast_to<int>() == 0) {
            throw DivisionByZeroError();
        }
        return value(dom_type, a.cast_to<int>() / b.cast_to<int>());
    }},
    {"%", [](value a, value b) -> value {
        std::string dom_type = get_dom_type(a, b);
        if (dom_type == "double")
            throw ModFromDoubleError();
        if (b.cast_to<int>() == 0)
            throw DivisionByZeroError();
        return value(dom_type, a.cast_to<int>() % b.cast_to<int>());
    }},
    {"<<", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.cast_to<int>() << b.cast_to<int>());
    }},
    {">>", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.cast_to<int>() >> b.cast_to<int>());
    }},
    {"&", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.cast_to<int>() & b.cast_to<int>());
    }},
    {"|", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.cast_to<int>() | b.cast_to<int>());
    }},
    {"^", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.cast_to<int>() ^ b.cast_to<int>());
    }},
    {"and", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.cast_to<int>() & b.cast_to<int>());
    }},
    {"or", [](value a, value b) -> value {
        if (get_dom_type(a, b) == "double")
            throw BitwiseOpsWithDoublesError();
        return value("int", a.cast_to<int>() | b.cast_to<int>());
    }},
    {"<", [](value a, value b) -> value { return value("bool", (get_dom_type(a, b) == "double") ?
        a.cast_to<double>() < b.cast_to<double>() : a.cast_to<int>() < b.cast_to<int>());} },
    {"<=", [](value a, value b) -> value { return value("bool", (get_dom_type(a, b) == "double") ? 
        a.cast_to<double>() <= b.cast_to<double>() : a.cast_to<int>() <= b.cast_to<int>());} },
    {">", [](value a, value b) -> value {return value("bool", (get_dom_type(a, b) == "double") ?
        a.cast_to<double>() > b.cast_to<double>() : a.cast_to<int>() > b.cast_to<int>());} },
    {">=", [](value a, value b) -> value {return value("bool", (get_dom_type(a, b) == "double") ?
        a.cast_to<double>() >= b.cast_to<double>() : a.cast_to<int>() >= b.cast_to<int>());} },
    {"==", [](value a, value b) -> value {return value("bool", (get_dom_type(a, b) == "double") ?
        a.cast_to<double>() == b.cast_to<double>() : a.cast_to<int>() == b.cast_to<int>());} },
    {"!=", [](value a, value b) -> value {return value("bool", (get_dom_type(a, b) == "double") ?
        a.cast_to<double>() != b.cast_to<double>() : a.cast_to<int>() != b.cast_to<int>());} }
};

void clear_last_value_stack() {
    while (!value_stack.back().empty()) {
        value_stack.back().pop_back();
    }
}

void pop_value_stack_to(value& a) {
    a = value_stack.back().back();
    value_stack.back().pop_back();
}

std::pair<bool, std::pair<std::string, std::string>> analize_util(const std::string& st) {
    if (st.size() < 4 || st.substr(0, 4) != "util")
        return {false, {}};
    std::string struct_type = "";
    std::string struct_name = "";
    size_t pos = 5;
    while (pos < st.size() && st[pos] == '_') {
        struct_type += st[pos++];
    }
    ++pos;
    while (pos < st.size() && st[pos] == '_') {
        struct_name += st[pos++];
    }
    return {true, {struct_type, struct_name}};
}

void InitializeGlobalVariables() {
    // Пока что глобал переменных нет
}

std::vector<char> special_symbols = {
    '\"', '\\', 'n'
};

void delete_memory(value& v) {
    if (!v.is_var()) {
        delete[] v.ptr.ptr;
        v.ptr.ptr = nullptr;
    }
}

void delete_memory(std::vector<value>& v) {
    for (value& u : v) {
        delete_memory(u);
    }
}

IdentifierTIDS GlobalIdTID;

// Пробегает через полиз пока не уткнётся в return/end_of_function/конец полиза
void Run(std::size_t run_from, Function* func_ptr = nullptr, 
        std::vector<value> vals = std::vector<value>(), std::string str_tp = NOT_A_TYPE, cool_byte* str_mem = nullptr) {
    value_stack.push_back({});
    IdentifierTIDS CurIdTIDS;
    if (func_ptr != nullptr) {
        CurIdTIDS.create_TID();
        const std::vector<Value>& arglist = func_ptr->argument_list();
        for (size_t ind = 0; ind < arglist.size(); ++ind) {
            const Value& v = arglist[ind];
            CurIdTIDS.cur_tid()->push_id({v.type(), v.name()}, StrTIDS);
            cool_byte* mem = CurIdTIDS.cur_tid()->find_id(v.name());
            std::memcpy(mem, vals[ind].ptr.ptr, get_type_size(vals[ind].tp, StrTIDS) * sizeof(cool_byte));
        }
    }
    
    for (std::size_t cur_pos = run_from; cur_pos < poliz.size();) {
        poliz_element cur_element = poliz[cur_pos++];
        switch (cur_element.type_info) {
            case LITERAL_OPERAND: {
                
                auto from = find(cur_element.maker.begin(), cur_element.maker.end(), '_');
                int literal_tp = std::stoi(cur_element.maker.substr(0, from - cur_element.maker.begin()));
                switch (literal_tp) {
                    case literal_int_type: {
                        int64_t result = std::stoll(cur_element.maker.substr(from - cur_element.maker.begin() + 1));
                        value_stack.back().emplace_back("int", result);
                        break;
                    }
                    case literal_double_type: {
                        double result = std::stod(cur_element.maker.substr(from - cur_element.maker.begin() + 1));
                        value_stack.back().emplace_back("double", result);
                        break;
                    }
                    case literal_bool_type: {
                        bool result = (cur_element.maker.substr(from - cur_element.maker.begin() + 1) == "true") ? true : false;
                        value_stack.back().emplace_back("bool", result);
                        break;
                    }
                    case string_literal_type: {
                        cur_element.maker = cur_element.maker.substr(from - cur_element.maker.begin() + 1);
                        cur_element.maker = cur_element.maker.substr(1, (int)cur_element.maker.size() - 2);
                        value_stack.back().emplace_back(cur_element.maker);
                        break;
                    }
                }
                break;
            }
            case OPERAND: {
                value_stack.back().emplace_back(cur_element.maker);
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
                        val.get_int_ref() += delta;
                    } else if (val.tp == "double") {
                        val.get_double_ref() += delta;
                    }
                    value_stack.back().push_back(val);
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
                    value_stack.back().push_back(val);
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
                    value_stack.back().push_back(val);
                } else if (cur_element.maker == "[]") {
                    value ind_elem, arr;
                    pop_value_stack_to(ind_elem);
                    pop_value_stack_to(arr);
                    string needed_type = get_array_type(arr.tp);
                    value result;
                    result.tp = needed_type;
                    result.is_var_ = true;
                    result.ptr.ptr = arr.ptr.ptr + get_type_size(result.tp, StrTIDS) * ind_elem.get_int_val();
                    value_stack.back().push_back(result);
                    delete_memory(ind_elem);
                    delete_memory(arr);
                } else if (cur_element.maker == ".") {
                    value used_struct, struct_member;
                    pop_value_stack_to(struct_member);
                    pop_value_stack_to(used_struct);
                    if (StrTIDS.check_id_safe(used_struct.tp, struct_member.name)) {
                        // it is a variable
                        value to_add;
                        to_add.tp = StrTIDS.check_id(used_struct.tp, struct_member.name);
                        to_add.is_var_ = true;
                        to_add.ptr.ptr = used_struct.ptr.ptr + 
                                StrTIDS.get_pointer_jump(used_struct.tp, struct_member.name);
                        value_stack.back().push_back(to_add);
                    } else {
                        // it is a function
                        value to_add(cur_element.maker);
                        to_add.tp = "function";
                        to_add.fun_info.func_ptr = StrTIDS.getFunction(used_struct.tp, struct_member.name);
                        to_add.fun_info.str_mem = used_struct.ptr.ptr;
                        to_add.fun_info.str_tp = used_struct.tp;
                        value_stack.back().push_back(to_add);
                    }
                    delete_memory(used_struct);
                    delete_memory(struct_member);
                } else if (cur_element.maker == "()") {
                    
                    std::vector<value> args;
                    while (value_stack.back().back().tp != "function") {
                        args.push_back(value_stack.back().back());
                        value_stack.back().pop_back();
                    }
                    
                    value func_val;
                    pop_value_stack_to(func_val);
                    std::reverse(args.begin(), args.end());
                    
                    Run(func_val.fun_info.func_ptr->poliz_pos(), func_val.fun_info.func_ptr,
                    args, func_val.fun_info.str_tp, func_val.fun_info.str_mem);
                    
                    delete_memory(args);
                    delete_memory(func_val);
                } else if (cur_element.maker == "input") {
                    value count_operands;
                    pop_value_stack_to(count_operands);
                    std::vector<value> vals(count_operands.get_int_val());
                    bool ok = true;
                    for (int ind = count_operands.get_int_val() - 1; ind >= 0; --ind) {
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
                    delete_memory(count_operands);
                } else if (cur_element.maker == "output") {
                    value count_operands;
                    pop_value_stack_to(count_operands);
                    std::vector<value> vals(count_operands.get_int_val());                    
                    bool ok = true;
                    for (int ind = count_operands.get_int_val() - 1; ind >= 0; --ind) {
                        pop_value_stack_to(vals[ind]);
                    }
                    for (const value& v : vals) {
                        if (v.tp == "int") {
                            std::cout << v.get_int_val();
                        } else if (v.tp == "bool") {
                            std::cout << v.get_bool_val();
                        } else if (v.tp == "double") {
                            std::cout << v.get_double_val();
                        } else if (!v.is_value()) {
                            bool note_special = false;
                            for (char c : v.name) {
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
                    delete_memory(count_operands);
                    delete_memory(vals);
                } else if (cur_element.maker == "," || cur_element.maker == ";") {
                    clear_last_value_stack();
                } else {
                    value arg1, arg2;
                    pop_value_stack_to(arg2);
                    pop_value_stack_to(arg1);
                    if (binary_rvalue_operations.count(cur_element.maker) == 0) {
                        if (cur_element.maker == "=") {
                            value_stack.back().push_back(arg1.assign(arg2));
                        } else {
                            std::string op = cur_element.maker.substr(0, cur_element.maker.size() - 1);
                            value_stack.back().push_back(arg1.assign(binary_rvalue_operations[op](arg1, arg2)));
                        }
                        delete_memory(arg2);
                    } else {
                        value_stack.back().push_back(binary_rvalue_operations[cur_element.maker](arg1, arg2));
                        delete_memory(arg1);
                        delete_memory(arg2);
                    }

                }
                break;
            }
            case UNCOND_MOVE: {
                value nxt_pos = value_stack.back().back();
                cur_pos = nxt_pos.get_int_val();
                value_stack.back().pop_back();
                delete_memory(nxt_pos);
                break; 
            }
            case MOVE_BY_FALSE: {
                value move_to = value_stack.back().back();
                value_stack.back().pop_back();
                value res = value_stack.back().back();
                value_stack.back().pop_back();
                if (!res.cast_to<bool>())
                    cur_pos = move_to.get_int_val();
                delete_memory(move_to);
                delete_memory(res);
                break;
            }
            case MOVE_BY_TRUE: {
                value move_to = value_stack.back().back();
                value_stack.back().pop_back();
                value res = value_stack.back().back();
                value_stack.back().pop_back();
                if (res.cast_to<bool>())
                    cur_pos = move_to.get_int_val();
                delete_memory(move_to);
                delete_memory(res);
                break;
            }
            case UNDEFINED: {
                break;
            }
            case FUNCTION_NAME: {
                value to_add;
                to_add.tp = "function";
                if (StrTIDS.check_func_id_safe(str_tp, cur_element.maker)) {
                    to_add.fun_info.func_ptr = StrTIDS.getFunction(str_tp, cur_element.maker);
                    to_add.fun_info.str_mem = str_mem;
                    to_add.fun_info.str_tp = str_tp;
                    value_stack.back().push_back(to_add);
                    break;
                }
                to_add.fun_info.func_ptr = FunTIDS.getFunction(cur_element.maker);
                to_add.fun_info.str_mem = nullptr;
                to_add.fun_info.str_tp = NOT_A_TYPE;
                value_stack.back().push_back(to_add);
                break;
            }
            case POSITION: {
                value_stack.back().emplace_back(value("int", cur_element.position));
                break;
            }
            case SEPARATOR: {
                // TO DO???
                break;
            }
            case REM_MATCH_MAIN_TERM: {
                main_match_terms.push_back(value_stack.back().back());
                value_stack.back().pop_back();
                break;
            }
            case GET_MATCH_MAIN_TERM: {
                value_stack.back().push_back(main_match_terms.back());
                break;
            }
            case DELETE_MATCH_MAIN_TERM: {
                value mm = main_match_terms.back();
                main_match_terms.pop_back();
                delete_memory(mm);
                break;
            }
            case MAKE_VARIABLE: {
                value tp, name;
                pop_value_stack_to(name);
                pop_value_stack_to(tp);
                CurIdTIDS.cur_tid()->push_id(Value(tp.name, name.name), StrTIDS);
                delete_memory(tp);
                delete_memory(name);
                break;
            }
            case MAKE_ARRAY_AUTO: {
                
                break;
            }
            case MAKE_ARRAY_EXACT: {
                value arr_sz, arr_name, arr_tp;
                std::vector<value> elems;
                value cur;
                while (pop_value_stack_to(cur), cur.is_value()) {
                    elems.push_back(cur);
                }
                arr_sz = elems.back();
                elems.pop_back();
                std::reverse(elems.begin(), elems.end());
                arr_name = cur;
                pop_value_stack_to(arr_tp);
                if (elems.size() > arr_sz.get_int_val()) {
                    throw TooManyElementsForArray(arr_name.name);
                }
                CurIdTIDS.cur_tid()->push_id({arr_tp.name + "_array", arr_name.name}, StrTIDS, arr_sz.get_int_val());
                cool_byte* ptr = CurIdTIDS.cur_tid()->find_id(arr_name.name);
                for (size_t ind = 0, tp_size = get_type_size(arr_tp.name, StrTIDS);
                     ind < elems.size(); ++ind) {
                    std::memcpy(ptr + ind * tp_size, elems[ind].ptr.ptr, tp_size * sizeof(cool_byte));
                }
                delete_memory(elems);
                delete_memory(arr_sz);
                delete_memory(arr_name);
                delete_memory(arr_tp);
                break;
            }
            case RETURN_OPERATOR: {
                value& cv = value_stack.back().back();
                value new_v = cv;
                new_v.ptr.ptr = nullptr;
                new_v.tp = func_ptr->return_type();
                new_v.copy(cv);
                value_stack.pop_back();
                if (new_v.tp != "void")
                    value_stack.back().push_back(new_v);
                delete_memory(cv);
                return;
            }
            case END_OF_FUNCTION: {
                value_stack.pop_back();
                return;
            }
            case VARIABLE_OPERAND: {
                if (CurIdTIDS.cur_tid()->check_id_safe(cur_element.maker)) {
                    value to_add;
                    to_add.ptr.ptr = CurIdTIDS.cur_tid()->find_id(cur_element.maker);
                    to_add.tp = CurIdTIDS.cur_tid()->check_id(cur_element.maker);
                    to_add.is_var_ = true;
                    value_stack.back().push_back(to_add);
                    break;
                }
                value to_add;
                to_add.ptr.ptr = str_mem + StrTIDS.get_pointer_jump(str_tp, cur_element.maker);
                to_add.tp = StrTIDS.check_id(str_tp, cur_element.maker);
                to_add.is_var_ = true;
                value_stack.back().push_back(to_add);
                break;
            }
            case CREATE_TID: {
                CurIdTIDS.create_TID();
                break;
            }
            case DELETE_TID: {
                CurIdTIDS.del_TID();
                break;
            }
            case CLEAR_STACK: {
                while (!value_stack.back().empty()) {
                    value val = value_stack.back().back();
                    value_stack.back().pop_back();
                    delete_memory(val);
                }
                break;
            }
            case STRUCT_MEMBER: {
                value_stack.back().push_back(value(cur_element.maker));
                break;
            }
        }
    }
}