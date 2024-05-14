#pragma once
#include "poliz.hpp"

poliz_element::poliz_element(ELEMENT_TYPE new_type) : type_info(new_type) {}

poliz_element::poliz_element(int position) : position(position), type_info(ELEMENT_TYPE::POSITION) {}

poliz_element::poliz_element(ELEMENT_TYPE new_type, const string& op_name) : type_info(new_type), maker(op_name) {}

void push_poliz(poliz_element new_element) {
    poliz.push_back(new_element);
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
