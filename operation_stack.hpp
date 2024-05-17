#include <string>
#include <vector>
#include "lexical_analyzer.h"
#include "common_functions.cpp"

enum STACK_ELEMENT_TYPE {
    RVALUE_S,
    LVALUE_S,
    OPERATOR_S,
    IDENTIFIER_S,
    FUNCTION_S,
    INPUT_END_S,
    OUTPUT_END_S
};

struct stack_element {
    STACK_ELEMENT_TYPE el_type;
    std::string type_info;
    std::string content;
    bool in_poliz = false;
    Function* func_ptr;
    size_t poliz_pos = -1;
    stack_element();
    // add in_poliz to constructors
    stack_element(STACK_ELEMENT_TYPE el_type_,
                  const std::string& type_info_,
                  const std::string& content_,
                  bool in_poliz_ = true);
    stack_element(STACK_ELEMENT_TYPE el_type_,
                  const std::string& type_info_,
                  const std::string& content_,
                  size_t poliz_pos_,
                  bool in_poliz_ = true);
};

std::vector<stack_element> elems;

void push_opstack(stack_element el);
stack_element pop_opstack();
void stack_clear();
std::string get_dominant_type(stack_element& f, stack_element& s);
void identify_as_lvalue(stack_element& f, std::string str = "");
void identify_as_function(stack_element& f);
bool is_basic_type(const std::string& s);
std::string get_array_type(const std::string& s);
bool is_array(const std::string& s);
void push_poliz_stack(const stack_element& s);
void push_remaining_elements_to_stack();

void check_bin();
void check_function_call();
void check_array_access();
void check_member_access();
void check_uno();
void check_assignment();
void check_input();
void check_output();
void check_left();

void eq_bool();
void eq_int();