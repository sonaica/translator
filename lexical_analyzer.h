#pragma once
#include <string>
#include <vector>


struct Vertex {
    Vertex *to[256] = {0};
    bool terminal = false;
};

struct Lexem {
    int type;
    std::string content;
};

struct Verdict {
    bool is_error = false;
    int type = -1;
    int line_number = -1;
    Lexem lexem;
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
bool is_keyword(std::vector<char> &text, int &i, std::string &keyword);
bool is_alphabet(char c);
Lexem create_lexem(std::string str, int type);
Verdict FSM();
