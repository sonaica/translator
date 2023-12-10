#include "syntax_analyzer.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "lexical_analyzer.h"

extern std::vector<char> text;
extern int pos;
extern int lines;
extern Lexem lexem;

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

Lexem request_lexem() {
    Verdict verdict = FSM();
    if (!verdict.is_error) {
        return verdict.lexem;
    }
    throw verdict;
}

// void OutputLexems(std::string file, std::vector<Lexem> &vec) {
//     std::ofstream out(file);
//     out << "1 - keyword\n";
//     out << "2 - identifier\n";
//     out << "3 - literal\n";
//     out << "4 - operation\n";
//     out << "5 - ; and {}\n";
//     out << "6 - ,\n";
//     out << "7 - ()\n\n";
//     if (out.is_open()) {
//         for (int i = 0; i < vec.size(); ++i) {
//             out << vec[i].content << " " << vec[i].type << '\n';
//         }
//     }
//     out.close();
// }

void Assignment() {
    Variable();
    Assignment_Operator();
    Expression();
}

void Assignment_Operator() {
    if (lexem.content != "=" && lexem.content != "<<=" &&
        lexem.content != ">>=" && lexem.content != "+=" &&
        lexem.content != "-=" && lexem.content != "*=" &&
        lexem.content != "**=" && lexem.content != "/=" &&
        lexem.content != "//=" && lexem.content != "^=" &&
        lexem.content != "&=" && lexem.content != "|=" &&
        lexem.content != "%=") {
        // throw
    }
    lexem = request_lexem();
}

void Comparison() {
    Expression();
    Comparisons_Operators();
    Expression();
}

void Comparisons_Operators() {
    if (lexem.content != ">" && lexem.content != "<" && lexem.content != ">=" &&
        lexem.content != "<=" && lexem.content != "==" &&
        lexem.content != "!=") {
        // throw
    }
    lexem = request_lexem();
}

void Operator() {
    if (lexem.content == "{") {
        while (lexem.content == "{") {
            lexem = request_lexem();
            Operator();
            if (lexem.content != "}") {
            }  // throw
            lexem = request_lexem();
        }
    } else {
        // CHEAT
        Lexem next = request_lexem();
        if (next.content == "if") {
            lexem = request_lexem();
            If();
        }
        if (next.content == "while") {
            lexem = request_lexem();
            While();
        }
        if (next.content == "for") {
            lexem = request_lexem();
            For();
        }
        if (next.content == "input") {
            lexem = request_lexem();
            Input();
        }
        if (next.content == "output") {
            lexem = request_lexem();
            Output();
        }
        if (next.content == "match") {
            lexem = request_lexem();
            Match();
        }
        // Expression operator
        // Variable creation
    }
    lexem = request_lexem();
}

void If() {
    if (lexem.content != "if") {
    }  // throw
    lexem = request_lexem();
    if (lexem.content != "(") {
    }  // throw
    lexem = request_lexem();
    Expression();
    if (lexem.content != ")") {
    }  // throw
    lexem = request_lexem();
    Operator();
    if (lexem.content == "else") {
        lexem = request_lexem();
        Operator();
    }
    lexem = request_lexem();
    // ??
}

void For() {
    if (lexem.content != "for") {
    }  // throw
}
