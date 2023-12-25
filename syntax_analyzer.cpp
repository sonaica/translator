#pragma once
#include "syntax_analyzer.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "CompileError.cpp"
#include "lexical_analyzer.cpp"
#include "op_stack.cpp"
#include "tids.cpp"
extern std::vector<char> text;
extern int pos;
extern Lexem lexem;
extern std::stack<std::string> st;

Verdict verdict;

IdentifierTIDS IdTIDS;
FunctionTIDS FunTIDS;
StructTIDS StrTIDS;

std::string function_in_creation;
std::string struct_in_creation;
std::string current_type;
Value variable_in_creation;

void ReadFile(std::string file, std::vector<char>& text) {
    std::ifstream in(file);
    if (in.is_open()) {
        in.seekg(0, in.end);
        int size = in.tellg();
        in.seekg(0, in.beg);

        char* buffer = new char[size];
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

void reset_variable_in_creation() {
    variable_in_creation.type_ = "";
    variable_in_creation.name_ = "";
}

void reset_function_in_creation() { function_in_creation = ""; }

void reset_struct_in_creation() { struct_in_creation = ""; }

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
        } else if (Type() != NOT_A_TYPE) {
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

void FunctionDefinition(std::string str) {
    if (lexem.content != "fun") throw InvalidFunctionDefinition();
    GetLexem();
    function_in_creation = Name();
    if (str == NOT_A_STRUCT)
        FunTIDS.push_func_id(function_in_creation);
    else
        StrTIDS.push_func_id(str, function_in_creation);
    FunctionParameters(str);
    if ((current_type = Type()) == NOT_A_TYPE) {
        current_type = Name();
        StrTIDS.check_struct_id(current_type);
    }
    if (str == NOT_A_STRUCT)
        FunTIDS.push_func_return_type(function_in_creation, current_type);
    else
        StrTIDS.push_func_return_type(str, function_in_creation, current_type);
    if (lexem.content != "{") throw InvalidFunctionDefinition();
    GetLexem();
    IdTIDS.create_TID();
    while (lexem.content != "}") {
        Operator();
    }
    IdTIDS.del_TID();
    GetLexem();
}

void FunctionParameters(std::string str) {
    if (lexem.content != "(") throw UndefinedFunctionParameters();
    GetLexem();
    if (lexem.content == ")") {
        GetLexem();
        return;
    }
    if ((current_type = Type()) == NOT_A_TYPE) {
        current_type = Name();
        StrTIDS.check_struct_id(current_type);
    }
    variable_in_creation.set_type(current_type);
    variable_in_creation.set_type(Name());
    if (str == NOT_A_STRUCT)
        FunTIDS.push_func_par(function_in_creation, variable_in_creation);
    else
        StrTIDS.push_func_par(str, function_in_creation, variable_in_creation);
    IdTIDS.cur_tid()->push_id(variable_in_creation);
    reset_variable_in_creation();
    while (lexem.content == ",") {
        GetLexem();
        if ((current_type = Type()) == NOT_A_TYPE) {
            current_type = Name();
            StrTIDS.check_struct_id(current_type);
        }
        variable_in_creation.set_type(current_type);
        variable_in_creation.set_type(Name());
        if (str == NOT_A_STRUCT)
            FunTIDS.push_func_par(function_in_creation, variable_in_creation);
        else
            StrTIDS.push_func_par(str, function_in_creation,
                                  variable_in_creation);
        IdTIDS.cur_tid()->push_id(variable_in_creation);
        reset_variable_in_creation();
    }
    reset_function_in_creation();
    if (lexem.content != ")") throw UndefinedFunctionParameters();
    GetLexem();
}

void FunctionCall(std::string str) {
    function_in_creation = Name();
    if (str == NOT_A_STRUCT)
        FunTIDS.check_func_id(function_in_creation);
    else
        StrTIDS.check_func_id(str, function_in_creation);
    ArgumentList(str);
    reset_function_in_creation();
}

void ArgumentList(std::string str) {
    if (lexem.content != "(") throw InvalidArgumentList();
    GetLexem();
    if (lexem.content == ")") {
        GetLexem();
        return;
    }
    int par_count = 0;
    Expression();
    if (str == NOT_A_STRUCT)
        FunTIDS.check_func_par(function_in_creation, par_count++, st.top());
    else
        StrTIDS.check_func_par(str, function_in_creation, par_count++,
                               st.top());
    stack_clear();
    while (lexem.content == ",") {
        GetLexem();
        Expression();
        if (str == NOT_A_STRUCT)
            FunTIDS.check_func_par(function_in_creation, par_count++, st.top());
        else
            StrTIDS.check_func_par(str, function_in_creation, par_count++,
                                   st.top());
        stack_clear();
    }
    if (str == NOT_A_STRUCT)
        FunTIDS.check_param_count(function_in_creation, par_count);
    else
        StrTIDS.check_param_count(str, function_in_creation, par_count);
    if (lexem.content != ")") throw InvalidArgumentList();
    GetLexem();
}

// Structures

void StructDefinition() {
    if (lexem.content != "struct") throw InvalidStructDefinition();
    GetLexem();
    struct_in_creation = Name();
    StrTIDS.push_struct_id(struct_in_creation);
    if (lexem.content != "{") throw InvalidStructDefinition();
    GetLexem();
    IdTIDS.create_TID();
    for (;;) {
        if (lexem.content == "fun")
            FunctionDefinition(struct_in_creation);
        else if (Type() != NOT_A_TYPE)
            VariableCreation(struct_in_creation);
        else
            break;
    }
    if (lexem.content != "}") throw InvalidStructDefinition();
    GetLexem();
    if (lexem.content == ";") {
        GetLexem();
        return;
    }
    variable_in_creation.set_name(Name());
    IdTIDS.cur_tid()->push_id(
        Value(struct_in_creation, variable_in_creation.name()));
    while (lexem.content == ",") {
        GetLexem();
        variable_in_creation.set_name(Name());
        IdTIDS.cur_tid()->push_id(
            Value(struct_in_creation, variable_in_creation.name()));
    }
    if (lexem.content != ";") throw InvalidStructDefinition();
    GetLexem();
}

void StructMember() {
    std::string mem = Name();
    struct_in_creation = "";
    std::string object;
    for (int ind = 0; ind < (int)mem.size(); ++ind) {
        if (ind == '.') {
            struct_in_creation = mem.substr(0, ind);
            object = mem.substr(ind + 1);
            break;
        }
    }
    if (lexem.content == "(") {
        function_in_creation = object;
        StrTIDS.check_func_id(struct_in_creation, function_in_creation);
        ArgumentList(struct_in_creation);
    }
    variable_in_creation.set_name(object);
    StrTIDS.check_id(struct_in_creation, variable_in_creation.name());
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

std::string Name() {
    if (lexem.type != identifier_type) throw InvalidName();
    // if (lexem.content.empty()) throw InvalidName();
    // if (Digit(lexem.content[0])) throw InvalidName();
    // for (int ind = 1; ind < lexem.content.size(); ++ind)
    //     if (!Letter(lexem.content[ind]) && !Digit(lexem.content[ind]))
    //         throw InvalidSymbol();
    std::string name = lexem.content;
    GetLexem();
    return name;
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

std::string Type() {
    for (const std::string& s : TypeList)
        if (lexem.content == s) {
            GetLexem();
            return s;
        }
    return NOT_A_TYPE;
}

void EntityCreation(std::string str) {
    variable_in_creation.set_type(Type());
    variable_in_creation.set_name(Name());
    if (lexem.content == "[") {
        GetLexem();

        variable_in_creation.set_type(variable_in_creation.type() + "_array");
        IdTIDS.cur_tid()->push_id(variable_in_creation);
        if (str != NOT_A_TYPE) StrTIDS.push_id(str, variable_in_creation);
        reset_variable_in_creation();

        ArrayDeclaration();
        return;
    }
    IdTIDS.cur_tid()->push_id(variable_in_creation);
    if (str != NOT_A_TYPE) StrTIDS.push_id(str, variable_in_creation);
    reset_variable_in_creation();

    if (lexem.content == "=") {
        GetLexem();
        Expression();
        return;
    }
}

void VariableCreation(std::string str) {
    EntityCreation(str);
    while (lexem.content == ",") {
        GetLexem();
        EntityCreation(str);
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
            std::string tmp = lexem.content;
            std::string type;
            GetLexem();
            int pos;
            for (pos = 0; pos < tmp.size(); ++pos) {
                if (tmp[pos] == '.') break;
            }
            if (pos == 0 || pos == (int)tmp.size() - 1) throw InvalidName();
            if (pos != (int)tmp.size()) {
                std::string str = tmp.substr(0, pos);
                std::string mem = tmp.substr(pos + 1);
                StrTIDS.check_struct_id(str);
                if (lexem.content == "(") {
                    StrTIDS.check_func_id(str, mem);
                    function_in_creation = mem;
                    ArgumentList(str);
                    type = StrTIDS.check_func_id(str, function_in_creation);
                } else {
                    type = StrTIDS.check_id(str, mem);
                }
                push_typeop(type);
            } else {
                if (lexem.content == "(") {
                    type = FunTIDS.check_func_id(tmp);
                    ArgumentList();
                    return;
                } else if (lexem.content == "[") {
                    type = IdTIDS.cur_tid()->check_id(tmp);
                    type = type.substr(0, type.find('_'));
                    GetLexem();
                    Expression();
                    eq_int();
                    if (lexem.content != "]") throw InvalidArrayIndexation();
                    GetLexem();
                } else {
                    type = IdTIDS.cur_tid()->check_id(tmp);
                }
                push_typeop(type);
            }
        } else {
            throw InvalidName();
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
        IdTIDS.create_TID();
        GetLexem();
        while (lexem.content != "}") {
            Operator();
        }
        IdTIDS.del_TID();
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
            if (lexem.content != ";") {
                Expression();
                if (lexem.content != ";") throw MissingSemicolumn();
            }
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
    IdTIDS.create_TID();
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
    IdTIDS.del_TID();
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
