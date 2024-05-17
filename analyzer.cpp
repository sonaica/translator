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

void print_poliz() {
    for (size_t ind = 0; ind < poliz.size(); ++ind) {
        std::cout << ind << ": ";
        std::cout << element_type_translation[poliz[ind].type_info];
        if (poliz[ind].type_info == ELEMENT_TYPE::POSITION)
            std::cout << " -> " << poliz[ind].position << "\n";
        else if (poliz[ind].maker.size())
            std::cout << " -> " << poliz[ind].maker << "\n";
        else
            std::cout << "\n";
    }
}

int main(int argc, char* argv[]) {
    using std::cout;
    // std::cout << "Enter the name of file: ";
    std::cout.precision(10);
    std::cout << std::fixed;
    std::string file_in = "in_cur.txt", file_out = "out.txt";
    const std::string file_keywords = "keywords.txt";
    CreateBor(file_keywords);
    ReadFile(file_in, text);
    try {
        GetLexem();
        size_t start = Program();
        try {
            if (argv[1][0] - '0' == 0) {
                std::cout << "MAIN START: " << start << "\n";
                print_poliz();
            }
            else Run(start);
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
    std::cout << "\nOK\n";
}
