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

Verdict verdict;

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
    if (lexem.content != ">" && lexem.content != ">" && lexem.content != ">=" &&
        lexem.content != "<=" && lexem.content != "==" &&
        lexem.content != "!=") {
        // throw
    }
    lexem = request_lexem();
}

void Operator() {
    if (lexem.content == "{") {
        lexem = request_lexem();
        while (lexem.content != "}") {
            Operator();
        }
    } else {
        lexem = request_lexem();
        if (lexem.content == "if") {
            If();
        }
        if (lexem.content == "while") {
            While();
        }
        if (lexem.content == "for") {
            For();
        }
        if (lexem.content == "input") {
            Input();
        }
        if (lexem.content == "output") {
            Output();
        }
        if (lexem.content == "match") {
            Match();
        }
        if (lexem.content == "int" || lexem.content == "void" ||
            lexem.content == "bool") {
            Variable_Creation();
            if (lexem.content != ";") {
                // throw
            }
        } else {
            Expression();
            if (lexem.content != ";") {
                // throw
            }
        }
    }
    lexem = request_lexem();
}

void If() {
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
}

void For() {
    lexem = request_lexem();
    if (lexem.content != "(") {
        // throw
    }
    lexem = request_lexem();
    Variable_Initialization();
    if (lexem.content != ";") {
        // throw
    }
    lexem = request_lexem();
    Expression();
    if (lexem.content != ";") {
        // throw
    }
    lexem = request_lexem();
    Expression();
    if (lexem.content != ")") {
        // throw
    }
    lexem = request_lexem();
    Operator();
}

void While() {
    lexem = request_lexem();
    if (lexem.content != "(") {
        // throw
    }
    lexem = request_lexem();
    Expression();
    if (lexem.content != ")") {
        // throw
    }
    lexem = request_lexem();
    if (lexem.content != "do") {
        // throw
    }
    lexem = request_lexem();
    Operator();
}

void Input() {
    lexem = request_lexem();
    Variable();
    while (lexem.content == ",") {
        lexem = request_lexem();
        Variable();
    }
    if (lexem.content != ";") {
        // throw
    }
    lexem = request_lexem();
}

void Output() {
    lexem = request_lexem();
    Expression();
    while (lexem.content == ",") {
        lexem = request_lexem();
        Expression();
    }
    if (lexem.content != ";") {
        // throw
    }
    lexem = request_lexem();
}

void Match() {
    lexem = request_lexem();
    Name();
    if (lexem.content != "{") {
        // throw
    }
    lexem = request_lexem();
    while (lexem.content != "}") {
        Expression();
        if (lexem.content != ">=") {
            // throw
        }
        Operator();
    }
    lexem = request_lexem();
}

void Array_Declaration() {
    Type();
    Name();
    if (lexem.content != "[") {
        // throw
    }
    lexem = request_lexem();
    if (lexem.content == "]") {
        lexem = request_lexem();
        Array_Declaration_Auto();
    } else {
        Array_Declaration_Exact();
    }
}

void Array_Declaration_Auto() {
    if (lexem.content != "=") {
        // throw
    }
    lexem = request_lexem();
    if (lexem.content != "{") {
        // throw
    }
    lexem = request_lexem();
    Literal();
    while (lexem.content == ",") {
        lexem = request_lexem();
        Literal();
    }
    if (lexem.content != "}") {
        // throw
    }
    lexem = request_lexem();
    if (lexem.content != ";") {
        // throw
    }
    lexem = request_lexem();
}

void Array_Declaration_Exact() {
    Expression();
    if (lexem.content != "]") {
        // throw
    }
    lexem = request_lexem();
    if (lexem.content == "=") {
        lexem = request_lexem();
        if (lexem.content != "{") {
            // throw
        }
        lexem = request_lexem();
        Literal();
        while (lexem.content == ",") {
            lexem = request_lexem();
            Literal();
        }
        if (lexem.content != "}") {
            // throw
        }
    }
    if (lexem.content != ";") {
        // throw
    }
    lexem = request_lexem();
}

void Array_Indexation() {
    Name();
    if (lexem.content != "[") {
        // throw
    }
    lexem = request_lexem();
    Expression();
    if (lexem.content != "]") {
        // throw
    }
    lexem = request_lexem();
}

void Literal() {
    lexem = request_lexem();
    if (lexem.content == "false" || lexem.content == "true") {
        Boolean_Literal();
    } else
        Arithmetic_Literal();
}