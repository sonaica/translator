#include "syntax_analyzer.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "SyntaxError.h"
#include "lexical_analyzer.h"
#include "stack.h"
extern std::vector<char> text;
extern int pos;
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
        lexem = verdict.lexem;
        return;
    }
    if (verdict.type == 1) {
        throw ImpossibleLexem();
    } else if (verdict.type == 2) {
        throw InvalidSymbol();
    } else if (verdict.type == 3) {
        throw InvalidStructMember();
    }
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
    Operator();
}

void Directive() {
    if (lexem.content == "import") {
        GetLexem();
        if (lexem.type != string_literal_type) throw InvalidFilename();
        GetLexem();
        if (lexem.content != ";") throw MissingSemicolumn();
        GetLexem();
        return;
    }
    if (lexem.content == "define") {
        GetLexem();
        String();
        String();
        if (lexem.content != ";") throw MissingSemicolumn();
        GetLexem();
        return;
    }
}

// Functions

void FunctionDefinition() {
    if (lexem.content != "fun") throw InvalidFunctionDefinition();
    GetLexem();
    Name();
    FunctionParameters();
    if (!Type()) Name();
    if (lexem.content != "{") throw InvalidFunctionDefinition();
    GetLexem();
    while (lexem.content != "}") {
        Operator();
    }
    GetLexem();
}

void FunctionParameters() {
    if (lexem.content != "(") throw UndefinedFunctionParameters();
    GetLexem();
    if (lexem.content == ")") {
        GetLexem();
        return;
    }
    if (!Type()) Name();
    Name();
    while (lexem.content == ",") {
        GetLexem();
        if (!Type()) Name();
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
    if (lexem.content == ")") {
        GetLexem();
        return;
    }
    Expression();
    stack_clear();
    while (lexem.content == ",") {
        GetLexem();
        Expression();
        stack_clear();
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
    if (lexem.content == ";") {
        GetLexem();
        return;
    }
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

bool SpecialSymbol(char c) { return c == '_' || c == '/' || c == ','; }

void Name() {
    if (lexem.type != identifier_type) throw InvalidName();
    // if (lexem.content.empty()) throw InvalidName();
    // if (Digit(lexem.content[0])) throw InvalidName();
    // for (int ind = 1; ind < lexem.content.size(); ++ind)
    //     if (!Letter(lexem.content[ind]) && !Digit(lexem.content[ind]))
    //         throw InvalidSymbol();
    GetLexem();
}

void Variable() {
    Name();
    if (lexem.content == "[") {
        GetLexem();
        Expression();
        if (lexem.content != "]") throw InvalidArrayIndexation();
        GetLexem();
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

void EntityCreation() {
    Type();
    Name();
    if (lexem.content == "[") {
        GetLexem();
        ArrayDeclaration();
        return;
    }
    if (lexem.content == "=") {
        GetLexem();
        Expression();
        return;
    }
}

void VariableCreation() {
    EntityCreation();
    while (lexem.content == ",") {
        GetLexem();
        EntityCreation();
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

void ArithmeticExpression() { AssignmentTerm(); }

bool Unary() {
    if (lexem.content == "+" || lexem.content == "-" || lexem.content == "++" ||
        lexem.content == "--" || lexem.content == "~") {
        push_typeop(lexem.content);
        GetLexem();
        return true;
    }
    return false;
}

bool Mul() {
    if (lexem.content == "*" || lexem.content == "/" || lexem.content == "%") {
        push_typeop(lexem.content);
        GetLexem();
        return true;
    }
    return false;
}

bool Sum() {
    if (lexem.content == "+" || lexem.content == "-") {
        push_typeop(lexem.content);
        GetLexem();
        return true;
    }
    return false;
}

bool Power() {
    if (lexem.content == "**") {
        push_typeop(lexem.content);
        GetLexem();
        return true;
    }
    return false;
}

bool And() {
    if (lexem.content == "&" || lexem.content == "and") {
        push_typeop(lexem.content);
        GetLexem();
        return true;
    }
    return false;
}

bool Xor() {
    if (lexem.content == "^") {
        push_typeop(lexem.content);
        GetLexem();
        return true;
    }
    return false;
}

bool Or() {
    if (lexem.content == "|" || lexem.content == "or") {
        push_typeop(lexem.content);
        GetLexem();
        return true;
    }
    return false;
}

bool Shift() {
    if (lexem.content == "<<" || lexem.content == ">>") {
        push_typeop(lexem.content);
        GetLexem();
        return true;
    }
    return false;
}

bool Equality() {
    if (lexem.content == "==" || lexem.content == "!=") {
        push_typeop(lexem.content);
        GetLexem();
        return true;
    }
    return false;
}

bool NonEquality() {
    if (lexem.content == "<" || lexem.content == ">" || lexem.content == "<=" ||
        lexem.content == ">=") {
        push_typeop(lexem.content);
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
        push_typeop(lexem.content);
        GetLexem();
        return true;
    }
    return false;
}

void UnaryTerm() {
    if (Unary()) {
        UnaryTerm();
        check_uno();
    } else {
        ArithmeticTerm();
    }
}

void PowerTerm() {
    UnaryTerm();
    while (Power()) {
        UnaryTerm();
        check_bin();
    }
}
// dopilit
void MulTerm() {
    PowerTerm();
    while (Mul()) {
        PowerTerm();
        check_bin();
    }
}

void SumTerm() {
    MulTerm();
    while (Sum()) {
        MulTerm();
        check_bin();
    }
}

void ShiftTerm() {
    SumTerm();
    while (Shift()) {
        SumTerm();
        check_bin();
    }
}

void NonEqualityTerm() {
    ShiftTerm();
    while (NonEquality()) {
        ShiftTerm();
        check_bin();
    }
}

void EqualityTerm() {
    NonEqualityTerm();
    while (Equality()) {
        NonEqualityTerm();
        check_bin();
    }
}

void AndTerm() {
    EqualityTerm();
    while (And()) {
        EqualityTerm();
        check_bin();
    }
}

void XorTerm() {
    AndTerm();
    while (Xor()) {
        AndTerm();
        check_bin();
    }
}

void OrTerm() {
    XorTerm();
    while (Or()) {
        XorTerm();
        check_bin();
    }
}

void AssignmentTerm() {
    OrTerm();
    while (Assignment()) {
        OrTerm();
        check_bin();
    }
}

void ArithmeticTerm() {
    if (lexem.content == "(") {
        GetLexem();
        Expression();
        if (lexem.content != ")") throw InvalidArithmeticTerm();
        GetLexem();
        return;
    }
    if (!BooleanLiteral() && !ArithmeticLiteral()) {
        if (lexem.type == identifier_type) {
            std::string type = check_id(lexem.content);
            push_typeop(type);
            GetLexem();
        } else
            throw InvalidName();
        if (lexem.content == "(") {
            ArgumentList();
            return;
        }
        if (lexem.content == "[") {
            GetLexem();
            Expression();
            eq_int();
            if (lexem.content != "]") throw InvalidArrayIndexation();
            GetLexem();
            return;
        }
    }
}

bool BooleanLiteral() {
    if (lexem.type == literal_bool_type) {
        push_typeop("bool");
        GetLexem();
        return true;
    }
    return false;
}

bool ArithmeticLiteral() {
    if (lexem.type == literal_int_type || lexem.type == literal_double_type) {
        if (lexem.type == literal_double_type)
            push_typeop("double");
        else
            push_typeop("int");
        GetLexem();
        return true;
    }
    return false;
    // if (!SignedNumber()) return false;
    // if (lexem.content == ".") {
    //     GetLexem();
    //     if (!UnsignedNumber()) throw InvalidArithmeticLiteral();
    // }
    // return true;
}

void Operator() {
    if (lexem.content == "{") {
        GetLexem();
        while (lexem.content != "}") {
            Operator();
        }
        GetLexem();
    } else {
        if (lexem.content == "if") {
            GetLexem();
            If();
            return;
        }
        if (lexem.content == "while") {
            GetLexem();
            While();
            return;
        }
        if (lexem.content == "for") {
            GetLexem();
            For();
            return;
        }
        if (lexem.content == "input") {
            GetLexem();
            Input();
            return;
        }
        if (lexem.content == "output") {
            GetLexem();
            Output();
            return;
        }
        if (lexem.content == "match") {
            GetLexem();
            Match();
            return;
        }
        if (lexem.content == "return") {
            GetLexem();
            Expression();
            if (lexem.content != ";") throw MissingSemicolumn();
            GetLexem();
            return;
        }
        if (lexem.content == "int" || lexem.content == "void" ||
            lexem.content == "bool" || lexem.content == "double") {
            VariableCreation();
            return;
        }
        Expression();
        stack_clear();
        if (lexem.content != ";") {
            throw MissingSemicolumn();
        }
        GetLexem();
    }
}

void If() {
    if (lexem.content != "(") {
        throw ExpectedOpenParenthesis();
    }
    GetLexem();
    Expression();
    eq_bool();
    stack_clear();
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
    if (lexem.content != "(") {
        throw ExpectedOpenParenthesis();
    }
    GetLexem();
    VariableCreation();
    Expression();
    stack_clear();
    if (lexem.content != ";") {
        throw MissingSemicolumn();
    }
    GetLexem();
    Expression();
    eq_bool();
    stack_clear();
    if (lexem.content != ")") {
        throw ExpectedCloseParenthesis();
    }
    GetLexem();
    Operator();
}

void While() {
    if (lexem.content != "(") {
        throw ExpectedOpenParenthesis();
    }
    GetLexem();
    Expression();
    eq_bool();
    stack_clear();
    if (lexem.content != ")") {
        throw ExpectedCloseParenthesis();
    }
    GetLexem();
    Operator();
}

void Input() {
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
    OutputItem();
    while (lexem.content == ",") {
        GetLexem();
        OutputItem();
    }
    if (lexem.content != ";") {
        throw MissingSemicolumn();
    }
    GetLexem();
}

void OutputItem() {
    if (lexem.type == string_literal_type) {
        GetLexem();
        return;
    }
    // if (lexem.content[0] == '\"' && lexem.content.back() == '\"') {
    //     String();
    //     return;
    // }
    if (lexem.content[0] == '\"') throw InvalidStringLiteral();
    Expression();
}

void Match() {
    Name();
    if (lexem.content != "{") {
        throw ExpectedFigureOpen();
    }
    GetLexem();
    while (lexem.content != "}") {
        Expression();
        stack_clear();
        if (lexem.content != ">=") {
            throw ExpectedMatch();
        }
        GetLexem();
        Operator();
    }
    GetLexem();
}

void ArrayDeclaration() {
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
}

void ArrayDeclarationExact() {
    Expression();
    stack_clear();
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
        GetLexem();
    }
}

void ArrayIndexation() {
    Name();
    if (lexem.content != "[") {
        throw ExpectedSquareOpen();
    }
    GetLexem();
    Expression();
    stack_clear();
    if (lexem.content != "]") {
        throw ExpectedSquareClose();
    }
    GetLexem();
}

void Literal() {
    if (lexem.content == "false" || lexem.content == "true") {
        BooleanLiteral();
    } else
        ArithmeticLiteral();
}
