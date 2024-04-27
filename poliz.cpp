#include "poliz.hpp"

poliz_element::poliz_element(ELEMENT_TYPE new_type) : type_info(new_type) {}

poliz_element::poliz_element(int position) : position(position), type_info(ELEMENT_TYPE::POSITION) {}

poliz_element::poliz_element(ELEMENT_TYPE new_type, const string& op_name) : type_info(new_type), maker(op_name) {}

void unconditional_move(int& ptr, int to) {
    ptr = to;
    return;
}

void move_if_false(int& ptr, int to, bool pred) {
    if (!pred) ptr = to;
    return;
}

void move_if_true(int& ptr, int to, bool pred) {
    if (pred) ptr = to;
    return;
}

void push_poliz(poliz_element new_element) {
    poliz.push_back(new_element);
}

ASSOCIATIVITY_TYPE get_associativity(const string& maker) {
    if (binary_search(associativity[0].begin(), associativity[0].end(), maker))
        return ASSOCIATIVITY_TYPE::RIGHT;
    return ASSOCIATIVITY_TYPE::LEFT;
}

void push_stack(poliz_element new_element)
{
    switch (new_element.type_info) {
        case OPERAND: {
            push_poliz(new_element);
            break;
        }
        case LITERAL_OPERAND: {
            push_poliz(new_element);
            break;
        }
        case VARIABLE_OPERAND: {
            push_poliz(new_element);
            break;
        }
        case CREATE_TID: {
            push_poliz(new_element);
            break;
        }
        case DELETE_TID: {
            push_poliz(new_element);
            break;
        }
        case PARENTHESIS: {
            if (new_element.maker == "(") {
                poliz_stack.push_back(new_element);
            } else if (new_element.maker == ")") {
                while (poliz_stack.size() && poliz_stack.back().maker != "(") {
                    push_poliz(poliz_stack.back());
                    pop_stack();
                }
                pop_stack();
                if (!poliz_stack.empty() && (poliz_stack.back().type_info == ELEMENT_TYPE::FUNCTION_NAME || 
                    poliz_stack.back().maker == "output" || poliz_stack.back().maker == "input")) {
                    push_poliz(poliz_stack.back());
                    pop_stack();
                }
            }
            break;
        }
        case FUNCTION_NAME: {
            poliz_stack.push_back(new_element);
            break;
        }
        case SEPARATOR: {
            if (new_element.maker == ",") {
                while (poliz_stack.size() && poliz_stack.back().maker != "(") {
                    push_poliz(poliz_stack.back());
                    pop_stack();
                }
            }
            break;
        }
        case OPERATOR: {
            if (get_associativity(new_element.maker) == ASSOCIATIVITY_TYPE::LEFT) {
                while (poliz_stack.size() && poliz_stack.back().type_info == ELEMENT_TYPE::OPERATOR
                        && priority[poliz_stack.back().maker] <= priority[new_element.maker]) {
                    push_poliz(poliz_stack.back());
                    pop_stack();
                }
                
            } else {
                while (poliz_stack.size() && poliz_stack.back().type_info == ELEMENT_TYPE::OPERATOR
                        && priority[poliz_stack.back().maker] < priority[new_element.maker]) {
                    push_poliz(poliz_stack.back());
                    pop_stack();
                }
            }
            poliz_stack.push_back(new_element);
            break;
        }
        case END_OF_FUNCTION: {
            push_poliz(new_element);
            break;
        }
        case MAKE_VARIABLE: {
            push_poliz(new_element);
            break;
        }
        case RETURN_OPERATOR: {
            push_poliz(new_element);
            break;
        }
        case MOVE_BY_FALSE: {
            push_poliz(new_element);
            break;
        }
        case MOVE_BY_TRUE: {
            push_poliz(new_element);
            break;
        }
        case UNCOND_MOVE: {
            push_poliz(new_element);
            break;
        }
        case POSITION: {
            push_poliz(new_element);
            break;
        }
        case REM_MATCH_CASE: {
            push_poliz(new_element);
            break;
        }
        case REM_MATCH_MAIN_TERM: {
            push_poliz(new_element);
            break;
        }
        case GET_CASE_TERM: {
            push_poliz(new_element);
            break;
        }
        case GET_MATCH_MAIN_TERM: {
            push_poliz(new_element);
            break;
        }
        case MAKE_ARRAY_AUTO: {
            push_poliz(new_element);
            break;
        }
        case MAKE_ARRAY_EXACT: {
            push_poliz(new_element);
            break;
        }
    }
}

void pop_stack()
{
    poliz_stack.pop_back();
}

size_t cur_ptr() {
    return poliz.size();
}

void poliz_blank() {
    poliz.push_back(poliz_element());
}

void PushRemainingOperators()
{
    while (!poliz_stack.empty()) {
        push_poliz(poliz_stack.back());
        pop_stack();
    }
}
