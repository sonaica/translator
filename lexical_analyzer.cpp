#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

const int bor_size = 256;

struct Vertex {
    Vertex *to[bor_size] = {0};
    bool terminal = false;
};

Vertex *root = new Vertex();

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

enum class States {
    H,
    Id,
    Operation,
    Literal_Int,
    Literal_Double,
};

struct Lexem {
    int type;
    std::string content;
};

struct Verdict {
    bool is_error = false;
    int type = -1;
    int line_number = -1;
};

bool is_operation(char c) {
    if (c == '=' || c == '+' || c == '-' || c == '~' || c == '&' || c == '|' ||
        c == '*' || c == '>' || c == '<' || c == '/' || c == '^' || c == '%')
        return true;
    return false;
}

bool is_letter(char c) {
    if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_') return true;
    return false;
}

bool is_digit(char c) {
    if (c >= '0' && c <= '9') return true;
    return false;
}

bool is_keyword(std::vector<char> &text, int &i, std::string &keyword) {
    auto v = root;
    int ind = i;
    std::string str;
    while (v != nullptr) {
        if (v->terminal) {
            keyword = str;
        }
        v = v->to[text[ind] - 'a'];
        if (v != nullptr) str += text[ind];
        ++ind;
    }
    if (str != keyword || str == "") {
        keyword.clear();
        return false;
    }
    i = ind - 1;
    return true;
}

bool is_alphabet(char c) {
    if (is_digit(c) || is_letter(c) || is_operation(c) || c == '(' ||
        c == ')' || c == '{' || c == '}' || c == ';' || c == ',' || c == '.')
        return true;
    return false;
}

void add_lexem(std::string &str, int type, std::vector<Lexem> &lexems) {
    Lexem lex;
    lex.content = str;
    lex.type = type;
    lexems.push_back(lex);
    str.clear();
}

Verdict FSM(std::vector<char> &text, std::vector<Lexem> &lexems) {
    States state = States::H;
    char cur;
    std::string current_lexem;
    Verdict verdict_return;
    int lines = 1;
    for (int i = 0; i < text.size();) {
        cur = text[i];
        switch (state) {
            case States::H:
                if (is_keyword(text, i, current_lexem)) {
                    if (current_lexem == "true" || current_lexem == "false")
                        add_lexem(current_lexem, 3, lexems);
                    else
                        add_lexem(current_lexem, 1, lexems);
                    continue;
                } else if (is_letter(cur)) {
                    current_lexem += cur;
                    ++i;
                    state = States::Id;
                } else if (is_digit(cur)) {
                    current_lexem += cur;
                    ++i;
                    state = States::Literal_Int;
                } else if (is_operation(cur)) {
                    current_lexem += cur;
                    ++i;
                    state = States::Operation;
                } else if (cur == ',') {
                    current_lexem += cur;
                    ++i;
                    add_lexem(current_lexem, 6, lexems);
                } else if (cur == '(' || cur == ')') {
                    current_lexem += cur;
                    ++i;
                    add_lexem(current_lexem, 7, lexems);
                } else if (cur == '{' || cur == '}' || cur == ';') {
                    current_lexem += cur;
                    ++i;
                    add_lexem(current_lexem, 5, lexems);
                } else if (cur == '\n') {
                    ++lines;
                    ++i;
                } else if (cur == ' ')
                    ++i;
                else if (is_alphabet(cur)) {
                    verdict_return.is_error = true;
                    verdict_return.line_number = lines;
                    verdict_return.type = 1;
                    return verdict_return;
                } else {
                    verdict_return.is_error = true;
                    verdict_return.type = 2;
                    verdict_return.line_number = lines;
                    return verdict_return;
                }
                break;
            case States::Id:
                if (is_letter(cur) || is_digit(cur)) {
                    current_lexem += cur;
                    ++i;
                } else {
                    add_lexem(current_lexem, 2, lexems);
                    state = States::H;
                }
                break;
            case States::Operation:
                if (is_operation(cur)) {
                    current_lexem += cur;
                    ++i;
                } else {
                    add_lexem(current_lexem, 4, lexems);
                    state = States::H;
                }
                break;
            case States::Literal_Int:
                if (is_digit(cur)) {
                    current_lexem += cur;
                    ++i;
                } else if (cur == '.') {
                    current_lexem += cur;
                    ++i;
                    state = States::Literal_Double;
                } else if (is_letter(cur)) {
                    verdict_return.is_error = true;
                    verdict_return.line_number = lines;
                    verdict_return.type = 1;
                    return verdict_return;
                } else {
                    add_lexem(current_lexem, 3, lexems);
                    state = States::H;
                }
                break;
            case States::Literal_Double:
                if (is_digit(cur)) {
                    current_lexem += cur;
                    ++i;
                } else {
                    add_lexem(current_lexem, 3, lexems);
                    state = States::H;
                }
                break;
        }
    }
    return verdict_return;
}

void ReadFile(std::string file, std::vector<char> &text) {
    std::ifstream in(file);
    if (in.is_open()) {
        in.seekg(0, in.end);
        int size = in.tellg();
        in.seekg(0, in.beg);

        char *buffer = new char[size];
        in.read(buffer, size);

        for (int i{}; i < size; ++i) {
            text.push_back(buffer[i]);
        }
        in.close();
        delete[] buffer;
    }
}

void OutputLexems(std::string file, std::vector<Lexem> &vec) {
    std::ofstream out(file);
    if (out.is_open()) {
        for (int i = 0; i < vec.size(); ++i) {
            out << vec[i].content << " " << vec[i].type << '\n';
        }
    }
    out.close();
}

int main() {
    // std::cout << "Enter the name of file: ";
    std::string file_in = "in.txt", file_out = "out.txt";
    std::string file_keywords = "keywords.txt";
    std::vector<char> text;
    std::vector<Lexem> lexems;
    CreateBor(file_keywords);
    ReadFile(file_in, text);

    auto verdict = FSM(text, lexems);
    if (verdict.is_error) {
        std::cout << "line " << verdict.line_number << ": ";
        if (verdict.type == 1) {
            std::cout << "Impossible to allocate a lexem";
        } else if (verdict.type == 2) {
            std::cout << "Invalid character";
        }
    } else {
        OutputLexems(file_out, lexems);
        std::cout << "OK";
    }
    return 0;
}
