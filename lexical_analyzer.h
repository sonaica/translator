#pragma once
#include <cstring>
#include <string>
#include <vector>
#include "CompileError.cpp"
#include <map>

const int keyword_type = 1;
const int identifier_type = 2;
const int literal_int_type = 3;
const int literal_double_type = 4;
const int literal_bool_type = 5;
const int string_literal_type = 6;
const int operation_type = 7;

struct Vertex {
    Vertex* to[256];
    bool terminal = false;
    Vertex();
};

struct Lexem {
    int type;
    std::string content;
    Lexem();
    Lexem(const std::string& content_, int type_);
};

struct Verdict {
    bool is_error = false;
    Lexem lexem;
    int type = -1;
};

enum class States {
    H,
    Id,
    Operation,
    Literal_Int,
    Literal_Double,
    Comment,
    Literal_String
};

void CreateBor(std::string file_keywords);
bool is_operation(char c);
bool is_letter(char c);
bool is_digit(char c);
bool is_keyword(std::vector<char>& text, int& i, std::string& keyword);
bool is_alphabet(char c);
Verdict FSM();

void update_line(bool f);
