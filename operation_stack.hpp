#include <string>
#include <vector>

enum STACK_ELEMENT_TYPE {
    RVALUE_S,
    LVALUE_S,
    OPERATOR_S,
    IDENTIFIER_S,
    FUNCTION_S
};

struct stack_element {
    STACK_ELEMENT_TYPE el_type;
    std::string type_info;
    std::string content;
    size_t poliz_pos = -1;
    stack_element();
    stack_element(STACK_ELEMENT_TYPE el_type_,
                  const std::string& type_info_,
                  const std::string& content_);
    stack_element(STACK_ELEMENT_TYPE el_type_,
                  const std::string& type_info_,
                  const std::string& content_,
                  size_t poliz_pos_);
};

std::vector<stack_element> elems;

void push_opstack(stack_element el, bool mode);
stack_element pop_opstack();
void stack_clear();
std::string get_dominant_type(stack_element& f, stack_element& s);
void identify_as_lvalue(stack_element& f);
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

void eq_bool();
void eq_int();

bool is_unary(stack_element op);
bool is_assignment(stack_element op);
bool is_comparison(stack_element op);
bool is_binary(stack_element op);
bool is_type(stack_element op);
bool is_member_access(stack_element op);