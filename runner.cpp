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

//**
Хранит элементы стека стеков для обработки полиза
*//
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

    void copy(const value& other) {
        if (ptr.is_null()) {
            ptr.ptr = new cool_byte;
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

void make_rvalue(value& val) {
    val.is_var_ = false;
}

// Функция, которая возвращает "доминирующий" тип данных
string get_dom_type(const value& a, const value& b) {
    if (a.tp == "double" || b.tp == "double") return "double";
    return "int";
}

value cast_to_type(value val, std::string tp) {
    make_rvalue(val);
    val.tp = tp;
    return val;
}

std::vector<std::vector<value>> value_stack;
std::vector<value> main_match_terms;

//**
Словарь, сопоставляющий бинарную операцию и соответствующую функцию
*//
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

template<class T>
T pop_from_stack(const std::string& tp) {
    value val = value_stack.back().back();
    T result;
    if (tp == "int")
        result = val.get_int_val();
    else if (tp == "double")
        result = val.get_double_val();
    else if (tp == "bool")
        result = val.get_bool_val();
    value_stack.back().pop_back();
    return result;
}

//**
написать
*//
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

// Пробегает через полиз пока не уткнётся в return/end_of_function/конец полиза, основная функция исполнения
// run_from - номер начального элемента для обработки
// function_name - имя обрабатываемой функции
// vals - список аргументов для функции
// str - структура, которой принадлежит функция (если есть)
// str_mem - блок памяти для этой структуры
void Run(std::size_t run_from, const std::string& function_name, 
        std::vector<value> vals = std::vector<value>(), std::string str = NOT_A_STRUCT, cool_byte* str_mem = nullptr) {
    value_stack.push_back({});
    if (function_name != "main") {
        Function* ff = FunTIDS.getFunction(function_name);
        for (size_t ind = 0; ind < vals.size(); ++ind) {
            std::string var_name = ff->argument_list()[ind].name();
            std::string var_type = ff->argument_list()[ind].get_type();
            IdTIDS.cur_tid()->push_id(Value(var_type, var_name), StrTIDS);
            cool_byte*& adr = IdTIDS.cur_tid()->find_id(var_name);
            adr = new cool_byte;
            *adr = *vals[ind].ptr.ptr;
        }
    }
    std::size_t cur_pos = run_from;
    while (cur_pos < poliz.size()) {
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
                    result.ptr.ptr = arr.ptr.ptr + get_type_size(arr.tp, StrTIDS) * ind_elem.get_int_val();
                    value_stack.back().push_back(result);
                } else if (cur_element.maker == ".") {
                    value used_struct, struct_member;
                    pop_value_stack_to(struct_member);
                    pop_value_stack_to(used_struct);
                    if (StrTIDS.check_id_safe(used_struct.tp, struct_member.utillity)) {
                        // it is a variable
                        value to_add;
                        to_add.tp = StrTIDS.check_id(used_struct.tp, struct_member.utillity);
                        to_add.is_var_ = true;
                        to_add.ptr.ptr = used_struct.ptr.ptr + 
                                StrTIDS.get_pointer_jump(used_struct.tp, struct_member.utillity);
                        value_stack.back().push_back(to_add);
                    } else {
                        // it is a function
                        value to_add(cur_element.maker);
                        to_add.tp += "_" + used_struct.tp + "_" + used_struct.utillity;
                        to_add.ptr.ptr = used_struct.ptr.ptr;
                    }
                } else if (cur_element.maker == "()") {
                    std::vector<value> vals;
                    std::pair<bool, std::pair<std::string, std::string>> st;
                    while (!(st = analize_util(value_stack.back().back().tp)).first) {
                        vals.push_back(value());
                        pop_value_stack_to(vals.back());
                    }
                    std::reverse(vals.begin(), vals.end());
                    Function* f = FunTIDS.getFunction(value_stack.back().back().utillity);
                    value_stack.back().pop_back();
                    IdTIDS.create_TID();
                    Run(f->poliz_pos(), f->name(), vals, st.second.first, IdTIDS.cur_tid()->find_id(st.second.second));
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
                    } else {
                        value_stack.back().push_back(binary_rvalue_operations[cur_element.maker](arg1, arg2));
                    }
                }
                break;
            }
            case UNCOND_MOVE: {
                cur_pos = value_stack.back().back().get_int_ref();
                value_stack.back().pop_back();
                break; 
            }
            case MOVE_BY_FALSE: {
                int move_to = value_stack.back().back().get_int_ref();
                value_stack.back().pop_back();
                bool res = value_stack.back().back().cast_to<bool>();
                value_stack.back().pop_back();
                if (!res)
                    cur_pos = move_to;
                break;
            }
            case MOVE_BY_TRUE: {
                int move_to = value_stack.back().back().get_int_ref();
                value_stack.back().pop_back();
                bool res = value_stack.back().back().cast_to<bool>();
                value_stack.back().pop_back();
                if (res)
                    cur_pos = move_to;
                break;
            }
            case UNDEFINED: {
                break;
            }
            case FUNCTION_NAME: {
                value_stack.back().push_back(value(cur_element.maker));
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
            case MAKE_VARIABLE: {
                value tp, name;
                pop_value_stack_to(name);
                pop_value_stack_to(tp);
                IdTIDS.cur_tid()->push_id(Value(tp.utillity, name.utillity), StrTIDS);
                break;
            }
            case MAKE_ARRAY_AUTO: {
                
                break;
            }
            case MAKE_ARRAY_EXACT: {
                
                value arr_sz, arr_name, arr_tp;
                std::vector<value> elems;
                value cur;
                while (pop_value_stack_to(cur), cur.tp != "util") {
                    elems.push_back(cur);
                }
                arr_sz = elems.back();
                elems.pop_back();
                std::reverse(elems.begin(), elems.end());
                arr_name = cur;
                pop_value_stack_to(arr_tp);
                if (elems.size() > arr_sz.get_int_val()) {
                    throw TooManyElementsForArray(arr_name.utillity);
                }
                IdTIDS.cur_tid()->push_id({arr_tp.utillity, arr_name.utillity}, StrTIDS, arr_sz.get_int_val());
                cool_byte* ptr = IdTIDS.cur_tid()->find_id(arr_name.utillity);
                for (size_t ind = 0, tp_size = get_type_size(arr_tp.utillity, StrTIDS);
                     ind < elems.size(); ++ind) {
                    std::memcpy(ptr + ind * tp_size, elems[ind].ptr.ptr, tp_size);
                }
                break;
            }
            case RETURN_OPERATOR: {
                
                value& cv = value_stack.back().back();
                value new_v = cv;
                new_v.ptr.ptr = nullptr;
                new_v.tp = FunTIDS.getFunction(function_name)->return_type();
                new_v.copy(cv);
                value_stack.pop_back();
                if (new_v.tp != "void")
                    value_stack.back().push_back(new_v);
                IdTIDS.del_TID();
                return;
            }
            case END_OF_FUNCTION: {
                IdTIDS.del_TID();
                value_stack.pop_back();
                return;
            }
            case VARIABLE_OPERAND: {
                if (IdTIDS.cur_tid()->check_id_safe(cur_element.maker)) {
                    value to_add;
                    to_add.ptr.ptr = IdTIDS.cur_tid()->find_id(cur_element.maker);
                    to_add.ptr.deleteable = false;
                    to_add.tp = IdTIDS.cur_tid()->check_id(cur_element.maker);
                    to_add.is_var_ = true;
                    value_stack.back().push_back(to_add);
                } else if (StrTIDS.check_id_safe(str, cur_element.maker)) {
                    value to_add;
                    to_add.ptr.ptr = str_mem + StrTIDS.get_pointer_jump(str, cur_element.maker);
                    to_add.ptr.deleteable = false;
                    to_add.tp = StrTIDS.check_id(str, cur_element.maker);
                    to_add.utillity = cur_element.maker;
                    to_add.is_var_ = true;
                    value_stack.back().push_back(to_add);
                }
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
            case CLEAR_STACK: {
                while (!value_stack.back().empty())
                    value_stack.back().pop_back();
                break;
            }
            case STRUCT_MEMBER: {
                value_stack.back().push_back(value(cur_element.maker));
                break;
            }
        }
    }
}
