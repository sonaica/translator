#include "syntax_analyzer.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "SyntaxError.h"
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

void GetLexem() {
    Verdict verdict = FSM();
    if (!verdict.is_error) {
        verdict.lexem;
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

// Program Structure

void Program() {
    for (;;) {
        if (lexem.content == "import" || lexem.content == "define")
            Directive();
        else
            break;
    }
    for (;;) {
        if (lexem.content == "fun") {
            FunctionDefinition();
        } else if (lexem.content == "struct") {
            StructDefinition();
        } else if (Type()) {
            VariableCreation();
        } else {
            break;
        }
    }
    if (lexem.content != "main") throw UndefinedMainFunctionError();
    GetLexem();
    if (lexem.content != "(") throw UndefinedMainFunctionError();
    GetLexem();
    if (lexem.content != ")") throw UndefinedMainFunctionError();
    GetLexem();
    if (lexem.content != "{") throw UndefinedMainFunctionError();
    GetLexem();
    Operator();
    if (lexem.content != "}") throw UndefinedMainFunctionError();
    GetLexem();
}

void Directive() {
    if (lexem.content == "import") {
        GetLexem();
        if (lexem.content.size() <= 2 || lexem.content[0] != '\"' ||
            lexem.content.back() != '\"')
            throw InvalidFilename();
        GetLexem();
        return;
    }
    if (lexem.content == "define") {
        GetLexem();
        String();
        String();
        return;
    }
}

// Functions

void FunctionDefinition() {
    if (lexem.content != "fun") throw InvalidFunctionDefinition();
    GetLexem();
    Name();
    FunctionParameters();
    Type();
    if (lexem.content != "{") throw InvalidFunctionDefinition();
    GetLexem();
    Operator();
    if (lexem.content != "return") throw InvalidFunctionDefinition();
    GetLexem();
    Expression();
    if (lexem.content != "}") throw InvalidFunctionDefinition();
    GetLexem();
}

void FunctionParameters() {
    if (lexem.content != "(") throw UndefinedFunctionParameters();
    GetLexem();
    Type();
    Name();
    while (lexem.content == ",") {
        GetLexem();
        Type();
        Name();
    }
    if (lexem.content != ")") throw UndefinedFunctionParameters();
    GetLexem();
}

void FunctionCall() {
    Name();
    ArgumentList();
}

void ArgumentList() {
    if (lexem.content != "(") throw InvalidArgumentList();
    GetLexem();
    Name();
    while (lexem.content == ",") {
        GetLexem();
        Name();
    }
    if (lexem.content != ")") throw InvalidArgumentList();
    GetLexem();
}

// Structures

void StructDefinition() {
    if (lexem.content != "struct") throw InvalidStructDefinition();
    GetLexem();
    Name();
    if (lexem.content != "{") throw InvalidStructDefinition();
    GetLexem();
    for (;;) {
        if (lexem.content == "fun")
            FunctionDefinition();
        else if (Type())
            VariableCreation();
        else
            break;
    }
    if (lexem.content != "}") throw InvalidStructDefinition();
    GetLexem();
    Name();
    while (lexem.content == ",") {
        GetLexem();
        Name();
    }
    if (lexem.content != ";") throw InvalidStructDefinition();
    GetLexem();
}

void StructMember() {
    Name();
    if (lexem.content != ".") throw InvalidStructMember();
    GetLexem();
    Name();
    if (lexem.content == "(") {
        ArgumentList();
    }
}

// Types and variables

void String() {
    for (char c : lexem.content) {
        Symbol(c);
    }
    GetLexem();
}

void Symbol(char c) {
    if (!Letter(c) && !Digit(c)) throw InvalidSymbol();
}

bool Letter(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || SpecialSymbol(c);
}

bool Digit(char c) { return ('0' <= c && c <= '9'); }

bool SpecialSymbol(char c) { return c == '_' || c == '/'; }

void Name() {
    if (lexem.content.empty()) throw InvalidName();
    if (Digit(lexem.content[0])) throw InvalidName();
    for (int ind = 1; ind < lexem.content.size(); ++ind)
        if (!Letter(lexem.content[ind]) && !Digit(lexem.content[ind]))
            throw InvalidSymbol();
}

void Variable() {
    Name();
    GetLexem();
    if (lexem.content == "[") {
        Expression();
        if (lexem.content != "]") throw InvalidArrayIndexation();
    }
}

bool Type() {
    for (const std::string &s : TypeList)
        if (lexem.content == s) {
            GetLexem();
            return true;
        }
    return false;
}

void VariableCreation() {
    Type();
    Name();
    if (lexem.content == "=") {
        GetLexem();
        Expression();
    }
    while (lexem.content == ",") {
        GetLexem();
        Name();
        if (lexem.content == "=") {
            GetLexem();
            Expression();
        }
    }
    if (lexem.content != ";") throw InvalidVariableCreation();
    GetLexem();
}

bool Sign() {
    if (lexem.content == "+" || lexem.content == "-") {
        GetLexem();
        return true;
    }
    return false;
}

bool SignedNumber() {
    if (!Sign()) {
        return UnsignedNumber();
    } else {
        GetLexem();
        return UnsignedNumber();
    }
}

bool UnsignedNumber() {
    for (char c : lexem.content) {
        if (!Digit(c)) return false;
    }
    GetLexem();
    return true;
}

// Expression

void Expression() {
    ExpressionTerm();
    while (lexem.content == ",") {
        GetLexem();
        ExpressionTerm();
    }
}

void ExpressionTerm() { ArithmeticExpression(); }

// Arithmetic, logic and comparison

void ArithmeticExpression() { UnaryTerm(); }

bool Unary() {
    if (lexem.content == "+" || lexem.content == "-" || lexem.content == "++" ||
        lexem.content == "--" || lexem.content == "~") {
        GetLexem();
        return true;
    }
    return false;
}

bool Mul() {
    if (lexem.content == "*" || lexem.content == "/" || lexem.content == "%") {
        GetLexem();
        return true;
    }
    return false;
}

bool Sum() {
    if (lexem.content == "+" || lexem.content == "-") {
        GetLexem();
        return true;
    }
    return false;
}

bool Power() {
    if (lexem.content == "**") {
        GetLexem();
        return true;
    }
    return false;
}

bool And() {
    if (lexem.content == "&" || lexem.content == "and") {
        GetLexem();
        return true;
    }
    return false;
}

bool Xor() {
    if (lexem.content == "^") {
        GetLexem();
        return true;
    }
    return false;
}

bool Or() {
    if (lexem.content == "|" || lexem.content == "or") {
        GetLexem();
        return true;
    }
    return false;
}

bool Shift() {
    if (lexem.content == "<<" || lexem.content == ">>") {
        GetLexem();
        return true;
    }
    return false;
}

bool Equality() {
    if (lexem.content == "==" || lexem.content == "!=") {
        GetLexem();
        return true;
    }
    return false;
}

bool NonEquality() {
    if (lexem.content == "<" || lexem.content == ">" || lexem.content == "<=" ||
        lexem.content == ">=") {
        GetLexem();
        return true;
    }
    return false;
}

bool Assignment() {
    if (lexem.content == "=" || lexem.content == "<<=" ||
        lexem.content == ">>=" || lexem.content == "+=" ||
        lexem.content == "-=" || lexem.content == "*=" ||
        lexem.content == "**=" || lexem.content == "/=" ||
        lexem.content == "//=" || lexem.content == "^=" ||
        lexem.content == "&=" || lexem.content == "|=" ||
        lexem.content == "%=") {
        GetLexem();
        return true;
    }
    return false;
}

void UnaryTerm() {
    if (Sign()) {
        UnaryTerm();
    } else {
        PowerTerm();
    }
}

void PowerTerm() {
    MulTerm();
    while (Power()) {
        MulTerm();
    }
}

void MulTerm() {
    SumTerm();
    while (Mul()) {
        SumTerm();
    }
}

void SumTerm() {
    ShiftTerm();
    while (Sum()) {
        ShiftTerm();
    }
}

void ShiftTerm() {
    NonEqualityTerm();
    while (Shift()) {
        NonEqualityTerm();
    }
}

void NonEqualityTerm() {
    EqualityTerm();
    while (NonEquality()) {
        EqualityTerm();
    }
}

void EqualityTerm() {
    AndTerm();
    while (Equality()) {
        AndTerm();
    }
}

void AndTerm() {
    XorTerm();
    while (And()) {
        XorTerm();
    }
}

void XorTerm() {
    OrTerm();
    while (Xor()) {
        OrTerm();
    }
}

void OrTerm() {
    AssignmentTerm();
    while (Or()) {
        AssignmentTerm();
    }
}

void AssignmentTerm() {
    ArithmeticTerm();
    while (Assignment()) {
        ArithmeticTerm();
    }
}

void ArithmeticTerm() {
    if (lexem.content == "(") {
        GetLexem();
        ArithmeticExpression();
        if (lexem.content != ")") throw InvalidArithmeticTerm();
        return;
    }
    if (!BooleanLiteral() && !ArithmeticLiteral()) {
        Name();
        if (lexem.content == "(") {
            ArgumentList();
            return;
        }
        if (lexem.content == ".") {
            GetLexem();
            Name();
            if (lexem.content == "(") {
                ArgumentList();
            }
            return;
        }
        if (lexem.content == "[") {
            Expression();
            if (lexem.content != "]") throw InvalidArrayIndexation();
            GetLexem();
            return;
        }
    }
}

bool BooleanLiteral() {
    if (lexem.content == "true" || lexem.content == "false") {
        GetLexem();
        return true;
    }
    return false;
}

bool ArithmeticLiteral() {
    if (!SignedNumber()) return false;
    if (lexem.content == ".") {
        GetLexem();
        if (!UnsignedNumber()) throw InvalidArithmeticLiteral();
    }
    return true;
}

void Operator() {
    if (lexem.content == "{") {
        GetLexem();
        while (lexem.content != "}") {
            Operator();
        }
    } else {
        GetLexem();
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
            VariableCreation();
            if (lexem.content != ";") {
                throw MissingSemicolumn();
            }
        } else {
            Expression();
            if (lexem.content != ";") {
                throw MissingSemicolumn();
            }
        }
    }
    GetLexem();
}

void If() {
    GetLexem();
    if (lexem.content != "(") {
        throw ExpectedOpenParenthesis();
    }
    GetLexem();
    Expression();
    if (lexem.content != ")") {
        throw ExpectedCloseParenthesis();
    }
    GetLexem();
    Operator();
    if (lexem.content == "else") {
        GetLexem();
        Operator();
    }
}

void For() {
    GetLexem();
    if (lexem.content != "(") {
        throw ExpectedOpenParenthesis();
    }
    GetLexem();
    VariableCreation();
    if (lexem.content != ";") {
        throw MissingSemicolumn();
    }
    GetLexem();
    Expression();
    if (lexem.content != ";") {
        throw MissingSemicolumn();
    }
    GetLexem();
    Expression();
    if (lexem.content != ")") {
        throw ExpectedCloseParenthesis();
    }
    GetLexem();
    Operator();
}

void While() {
    GetLexem();
    if (lexem.content != "(") {
        throw ExpectedOpenParenthesis();
    }
    GetLexem();
    Expression();
    if (lexem.content != ")") {
        throw ExpectedCloseParenthesis();
    }
    GetLexem();
    if (lexem.content != "do") {
        throw ExpectedDo();
    }
    GetLexem();
    Operator();
}

void Input() {
    GetLexem();
    Variable();
    while (lexem.content == ",") {
        GetLexem();
        Variable();
    }
    if (lexem.content != ";") {
        throw MissingSemicolumn();
    }
    GetLexem();
}

void Output() {
    GetLexem();
    Expression();
    while (lexem.content == ",") {
        GetLexem();
        Expression();
    }
    if (lexem.content != ";") {
        throw MissingSemicolumn();
    }
    GetLexem();
}

void Match() {
    GetLexem();
    Name();
    if (lexem.content != "{") {
        throw ExpectedFigureOpen();
    }
    GetLexem();
    while (lexem.content != "}") {
        Expression();
        if (lexem.content != ">=") {
            throw ExpectedMatch();
        }
        Operator();
    }
    GetLexem();
}

void ArrayDeclaration() {
    Type();
    Name();
    if (lexem.content != "[") {
        throw ExpectedSquareOpen();
    }
    GetLexem();
    if (lexem.content == "]") {
        GetLexem();
        ArrayDeclarationAuto();
    } else {
        ArrayDeclarationExact();
    }
}

void ArrayDeclarationAuto() {
    if (lexem.content != "=") {
        throw ExpectedEqual();
    }
    GetLexem();
    if (lexem.content != "{") {
        throw ExpectedFigureOpen();
    }
    GetLexem();
    Literal();
    while (lexem.content == ",") {
        GetLexem();
        Literal();
    }
    if (lexem.content != "}") {
        throw ExpectedFigureClose();
    }
    GetLexem();
    if (lexem.content != ";") {
        throw MissingSemicolumn();
    }
    GetLexem();
}

void ArrayDeclarationExact() {
    Expression();
    if (lexem.content != "]") {
        throw ExpectedSquareClose();
    }
    GetLexem();
    if (lexem.content == "=") {
        GetLexem();
        if (lexem.content != "{") {
            throw ExpectedFigureOpen();
        }
        GetLexem();
        Literal();
        while (lexem.content == ",") {
            GetLexem();
            Literal();
        }
        if (lexem.content != "}") {
            throw ExpectedFigureClose();
        }
    }
    if (lexem.content != ";") {
        throw MissingSemicolumn();
    }
    GetLexem();
}

void ArrayIndexation() {
    Name();
    if (lexem.content != "[") {
        throw ExpectedSquareOpen();
    }
    GetLexem();
    Expression();
    if (lexem.content != "]") {
        throw ExpectedSquareClose();
    }
    GetLexem();
}

void Literal() {
    GetLexem();
    if (lexem.content == "false" || lexem.content == "true") {
        BooleanLiteral();
    } else
        ArithmeticLiteral();
}
