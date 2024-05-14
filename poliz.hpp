#pragma once
#include <vector>
#include <string>
#include <map>
#include <algorithm>
using std::vector;
using std::string;
using std::map;

enum ELEMENT_TYPE {
    LITERAL_OPERAND,
    OPERAND,
    OPERATOR,
    UNCOND_MOVE,
    MOVE_BY_FALSE,
    MOVE_BY_TRUE,
    UNDEFINED,
    FUNCTION_NAME,
    PARENTHESIS,
    POSITION,
    SEPARATOR,
    REM_MATCH_MAIN_TERM,
    GET_MATCH_MAIN_TERM,
    MAKE_VARIABLE,
    MAKE_ARRAY_EXACT,
    MAKE_ARRAY_AUTO,
    RETURN_OPERATOR,
    END_OF_FUNCTION,
    VARIABLE_OPERAND,
    CREATE_TID,
    DELETE_TID,
    CLEAR_STACK,
    STOP_CLEARING_STACK,
    STRUCT_MEMBER
};

std::map<ELEMENT_TYPE, std::string> element_type_translation = {
    {ELEMENT_TYPE::LITERAL_OPERAND, "LITERAL_OPERAND"},
    {ELEMENT_TYPE::OPERAND, "OPERAND"},
    {ELEMENT_TYPE::OPERATOR, "OPERATOR"},
    {ELEMENT_TYPE::UNCOND_MOVE, "UNCONDITIONAL_MOVE"},
    {ELEMENT_TYPE::MOVE_BY_FALSE, "MOVE_IF_FALSE"},
    {ELEMENT_TYPE::MOVE_BY_TRUE, "MOVE_IF_TRUE"},
    {ELEMENT_TYPE::UNDEFINED, "UNDEFINED"},
    {ELEMENT_TYPE::FUNCTION_NAME, "FUNCTION_NAME"},
    {ELEMENT_TYPE::PARENTHESIS, "PARENTHESIS"},
    {ELEMENT_TYPE::POSITION, "POSITION"},
    {ELEMENT_TYPE::SEPARATOR, "SEPARATOR"},
    {ELEMENT_TYPE::REM_MATCH_MAIN_TERM, "REMEMBER_MAIN_TERM_OF_MATCH"},
    {ELEMENT_TYPE::GET_MATCH_MAIN_TERM, "GET_MATCH_MAIN_TERM"},
    {ELEMENT_TYPE::MAKE_VARIABLE, "MAKE_VARIABLE"},
    {ELEMENT_TYPE::MAKE_ARRAY_EXACT, "MAKE_ARRAY_EXACT"},
    {ELEMENT_TYPE::MAKE_ARRAY_AUTO, "MAKE_ARRAY_AUTO"},
    {ELEMENT_TYPE::RETURN_OPERATOR, "RETURN_OPERATOR"},
    {ELEMENT_TYPE::END_OF_FUNCTION, "END_OF_FUNCTION"},
    {ELEMENT_TYPE::VARIABLE_OPERAND, "VARIABLE_OPERAND"},
    {ELEMENT_TYPE::CREATE_TID, "CREATE_NEW_TID"},
    {ELEMENT_TYPE::DELETE_TID, "DELETE_CURRENT_TID"},
    {ELEMENT_TYPE::CLEAR_STACK, "CLEAR_STACK"},
    {ELEMENT_TYPE::STRUCT_MEMBER, "STRUCT_MEMBER"}
};

enum ASSOCIATIVITY_TYPE {
    RIGHT,
    LEFT
};

struct poliz_element;

struct poliz_element {
    ELEMENT_TYPE type_info;
    int position;
    string maker; // function/parenthesis/operator

    poliz_element(ELEMENT_TYPE new_type = UNDEFINED);
    poliz_element(int position);
    poliz_element(ELEMENT_TYPE new_type, const string& op_name);
};


vector<poliz_element> poliz;
vector<poliz_element> poliz_stack;

ASSOCIATIVITY_TYPE get_associativity(const string& maker);

void push_poliz(poliz_element new_element);

void pop_stack();

size_t cur_ptr();

void poliz_blank();
