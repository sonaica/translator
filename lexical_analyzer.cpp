#include "lexical_analyzer.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

Vertex *root = new Vertex();

extern std::vector<char> text;
extern int pos;
extern int lines;

void CreateBor(std::string file_keywords) {
    std::string str;
    std::ifstream in(file_keywords);
    while (getline(in, str)) {
        auto v = root;
        for (char c : str) {
            c -= 'a';
            if (!v->to[c]) v->to[c] = new Vertex();
            v = v->to[c];
        }
        v->terminal = true;
    }
}

bool is_operation(char c) {
    if (c == '=' || c == '+' || c == '-' || c == '~' || c == '&' || c == '|' ||
        c == '*' || c == '>' || c == '<' || c == '/' || c == '^' || c == '%' ||
        c == '.')
        return true;
    return false;
}

bool is_letter(char c) {
    if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_' || c == '\\')
        return true;
    return false;
}

bool is_digit(char c) {
    if (c >= '0' && c <= '9') return true;
    return false;
}

bool is_keyword(std::string keyword) {
    auto v = root;
    for (int i = 0; i < keyword.length(); ++i) {
        if (v == nullptr) return false;
        v = v->to[keyword[i] - 'a'];
    }
    if (v != nullptr && v->terminal) return true;
    return false;
}

bool is_alphabet(char c) {
    if (is_digit(c) || is_letter(c) || is_operation(c) || c == '(' ||
        c == ')' || c == '{' || c == '}' || c == ';' || c == ',' || c == '.')
        return true;
    return false;
}

Lexem create_lexem(std::string str, int type) {
    Lexem lex;
    lex.content = str;
    lex.type = type;
    return lex;
}

Verdict FSM() {
    Lexem lex;
    States state = States::H;
    char cur;
    std::string current_lexem;
    Verdict verdict_return;
    while (pos < text.size()) {
        cur = text[pos];
        switch (state) {
            case States::H:
                if (cur == '/' && pos + 1 < text.size() &&
                    text[pos + 1] == '/') {
                    pos += 2;
                    state = States::Comment;
                } else if (is_letter(cur)) {
                    current_lexem += cur;
                    ++pos;
                    state = States::Id;
                } else if (is_digit(cur)) {
                    current_lexem += cur;
                    ++pos;
                    state = States::Literal_Int;
                } else if (is_operation(cur)) {
                    current_lexem += cur;
                    ++pos;
                    state = States::Operation;
                } else if (cur == ',') {
                    current_lexem += cur;
                    ++pos;
                    verdict_return.lexem = create_lexem(current_lexem, 6);
                    return verdict_return;
                } else if (cur == '(' || cur == ')') {
                    current_lexem += cur;
                    ++pos;
                    verdict_return.lexem = create_lexem(current_lexem, 7);
                    return verdict_return;
                } else if (cur == '[' || cur == ']') {
                    current_lexem += cur;
                    ++pos;
                    verdict_return.lexem = create_lexem(current_lexem, 7);
                    return verdict_return;
                } else if (cur == '{' || cur == '}' || cur == ';') {
                    current_lexem += cur;
                    ++pos;
                    verdict_return.lexem = create_lexem(current_lexem, 5);
                    return verdict_return;
                } else if (cur == '\n') {
                    ++lines;
                    ++pos;
                } else if (cur == ' ')
                    ++pos;
                else if (cur == '\"') {
                    current_lexem += cur;
                    ++pos;
                    state = States::Literal_String;
                } else if (is_alphabet(cur)) {
                    verdict_return.is_error = true;
                    verdict_return.type = 1;
                    return verdict_return;
                } else {
                    verdict_return.is_error = true;
                    verdict_return.type = 2;
                    return verdict_return;
                }
                break;
            case States::Id:
                if (is_letter(cur) || is_digit(cur)) {
                    current_lexem += cur;
                    ++pos;
                } else {
                    if (is_keyword(current_lexem)) {
                        if (current_lexem == "true" ||
                            current_lexem == "false") {
                            verdict_return.lexem =
                                create_lexem(current_lexem, 3);
                            return verdict_return;
                        } else {
                            verdict_return.lexem =
                                create_lexem(current_lexem, 1);
                            return verdict_return;
                        }
                    }
                    verdict_return.lexem = create_lexem(current_lexem, 2);
                    return verdict_return;
                }
                break;
            case States::Operation:
                if (is_operation(cur)) {
                    current_lexem += cur;
                    ++pos;
                } else {
                    verdict_return.lexem = create_lexem(current_lexem, 4);
                    return verdict_return;
                }
                break;
            case States::Literal_Int:
                if (is_digit(cur)) {
                    current_lexem += cur;
                    ++pos;
                } else if (cur == '.') {
                    current_lexem += cur;
                    ++pos;
                    state = States::Literal_Double;
                } else if (is_letter(cur)) {
                    verdict_return.is_error = true;
                    verdict_return.type = 1;
                    return verdict_return;
                } else {
                    verdict_return.lexem = create_lexem(current_lexem, 3);
                    return verdict_return;
                }
                break;
            case States::Literal_Double:
                if (is_digit(cur)) {
                    current_lexem += cur;
                    ++pos;
                } else {
                    verdict_return.lexem = create_lexem(current_lexem, 3);
                    return verdict_return;
                }
                break;
            case States::Comment:
                if (cur != '\n')
                    ++pos;
                else
                    state = States::H;
                break;
            case States::Literal_String:
                if (cur != '\"') {
                    current_lexem += cur;
                    ++pos;
                } else {
                    current_lexem += cur;
                    ++pos;
                    verdict_return.lexem = create_lexem(current_lexem, 3);
                    return verdict_return;
                }
                break;
        }
    }
    return verdict_return;
}