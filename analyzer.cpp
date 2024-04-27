#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "CompileError.cpp"
#include <vector>

#include "lexical_analyzer.cpp"
#include "syntax_analyzer.cpp"
#include "runner.cpp"

int pos = 0;
int lines = 1;
std::vector<char> text;
Lexem lexem;
bool new_line, new_line_prev;

int main() {
    using std::cout;
    // std::cout << "Enter the name of file: ";
    std::string file_in = "in_cur.txt", file_out = "out.txt";
    const std::string file_keywords = "keywords.txt";
    CreateBor(file_keywords);
    ReadFile(file_in, text);
    try {
        GetLexem();
        size_t start = Program();
        try {
            Run(start, "main");
        } catch (RuntimeError& e) {
            e.what();
            return 1;
        }
    } catch (CompileError& e) {
        if (new_line_prev) --lines;
        std::cout << "in line " << lines << ": ";
        e.what();
        cout << '\n';
        return 1;
    }
    std::cout << "OK\n";
}
