#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "CompileError.cpp"
#include <vector>

#include "lexical_analyzer.cpp"
#include "syntax_analyzer.cpp"

int pos = 0;
int lines = 1;
std::vector<char> text;
Lexem lexem;
bool new_line, new_line_prev;

int main() {
    // std::cout << "Enter the name of file: ";
    std::string file_in = "in2.txt", file_out = "out.txt";
    const std::string file_keywords = "keywords.txt";
    CreateBor(file_keywords);
    ReadFile(file_in, text);

    try {
        GetLexem();
        Program();
    } catch (CompileError& e) {
        if (new_line_prev) --lines;
        std::cout << "in line " << lines << ": ";
        e.what();
        return 1;
    }
    std::cout << "OK";
    return 0;
}
