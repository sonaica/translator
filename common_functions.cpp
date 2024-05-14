#pragma once
#include <string>

//**
В этом файле описаны функции, общие для нескольких файлов. 
*//

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
