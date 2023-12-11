#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "SyntaxError.h"
#include "lexical_analyzer.h"
#include "syntax_analyzer.h"

int pos = 0;
int lines = 1;
std::vector<char> text;
Lexem lexem;

int main() {
    // std::cout << "Enter the name of file: ";
    std::string file_in = "in.txt", file_out = "out.txt";
    const std::string file_keywords = "keywords.txt";
    CreateBor(file_keywords);
    ReadFile(file_in, text);

    try {
        GetLexem();
        Program();
    } catch (SyntaxError& e) {
        std::cout << "in line " << lines << ": ";
        e.what();
        return 1;
    }
    std::cout << "OK";
    return 0;
}
