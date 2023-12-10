#include "SintaxAnalyzer.hpp"
#include <iostream>

// General

void GetLexem() 
{
    // CurLexem = ...
}

void Analyze() 
{
    try {
        GetLexem();
        Program();
    }
    catch (...) {
        std::cout << "Oh shit here we go again";
    }
}

// Program Structure

void Program() 
{
    for (;;) {
        if (CurLexem.content == "import" || CurLexem.content == "define")
            Directive();
        else
            break;
    }
    for (;;) {
        if (CurLexem.content == "fun") {
            FunctionDefinition();
        }
        else if (CurLexem.content == "struct") {
            StructDefinition();
        }
        else if (Type()) {
            VariableCreation();
        }
        else {
            break;
        }
    }
    if (CurLexem.content != "main")
        throw UndefinedMainFunctionError();
    GetLexem();
    if (CurLexem.content != "(")
        throw UndefinedMainFunctionError();
    GetLexem();
    if (CurLexem.content != ")")
        throw UndefinedMainFunctionError();
    GetLexem();
    if (CurLexem.content != "{")
        throw UndefinedMainFunctionError();
    GetLexem();
    Operator();
    if (CurLexem.content != "}")
        throw UndefinedMainFunctionError();
    GetLexem();
}

void Directive()
{
    if (CurLexem.content == "import") {
        GetLexem();
        if (CurLexem.content.size() <= 2 || CurLexem.content[0] != '\"' || CurLexem.content.back() != '\"')
            throw InvalidFilename();
        GetLexem();
        return;
    }
    if (CurLexem.content == "define") {
        GetLexem();
        String();
        String();
        return;
    }
}

// Functions

void FunctionDefinition() 
{
    if (CurLexem.content != "fun")
        throw InvalidFunctionDefinition();
    GetLexem();
    Name();
    FunctionParameters();
    Type();
    if (CurLexem.content != "{")
        throw InvalidFunctionDefinition();
    GetLexem();
    Operator();
    if (CurLexem.content != "return")
        throw InvalidFunctionDefinition();
    GetLexem();
    Expression();
    if (CurLexem.content != "}")
        throw InvalidFunctionDefinition();
    GetLexem();
}

void FunctionParameters() {
    if (CurLexem.content != "(")
        throw UndefinedFunctionParameters();
    GetLexem();
    Type();
    Name();
    while (CurLexem.content == ",") {
        GetLexem();
        Type();
        Name();
    }
    if (CurLexem.content != ")")
        throw UndefinedFunctionParameters();
    GetLexem();
}

void FunctionCall() {
    Name();
    ArgumentList();
}

void ArgumentList() {
    if (CurLexem.content != "(")
        throw InvalidArgumentList();
    GetLexem();
    Name();
    while (CurLexem.content == ",") {
        GetLexem();
        Name();
    }
    if (CurLexem.content != ")")
        throw InvalidArgumentList();
    GetLexem();
}

// Structures

void StructDefinition() 
{
    if (CurLexem.content != "struct")
        throw InvalidStructDefinition();
    GetLexem();
    Name();
    if (CurLexem.content != "{")
        throw InvalidStructDefinition();
    GetLexem();
    for (;;) {
        if (CurLexem.content == "fun")
            FunctionDefinition();
        else if (Type())
            VariableCreation();
        else
            break;
    }
    if (CurLexem.content != "}")
        throw InvalidStructDefinition();
    GetLexem();
    Name();
    while (CurLexem.content == ",") {
        GetLexem();
        Name();
    }
    if (CurLexem.content != ";")
        throw InvalidStructDefinition();
    GetLexem();
}

void StructMember() {
    Name();
    if (CurLexem.content != ".")
        throw InvalidStructMember();
    GetLexem();
    Name();
    if (CurLexem.content == "(") {
        ArgumentList();
    }
}

// Types and variables

void String() {
    for (char c : CurLexem.content) {
        Symbol(c);
    }
    GetLexem();
}

void Symbol(char c) {
    if (!Letter(c) && !Digit(c))
        throw InvalidSymbol();
}

bool Letter(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || SpecialSymbol(c);
}

bool Digit(char c) {
    return ('0' <= c && c <= '9');
}

bool SpecialSymbol(char c) {
    return c == '_' || c == '/';
}

void Name() {
    if (CurLexem.content.empty())
        throw InvalidName();
    if (Digit(CurLexem.content[0]))
        throw InvalidName();
    for (int ind = 1; ind < CurLexem.content.size(); ++ind)
        if (!Letter(CurLexem.content[ind]) && !Digit(CurLexem.content[ind]))
            throw InvalidSymbol();
}

void Variable() {
    Name();
    GetLexem();
    if (CurLexem.content == "[") {
        Expression();
        if (CurLexem.content != "]")
            throw InvalidArrayIndexation();
    }
}

bool Type() {
    for (const std::string& s : TypeList)
        if (CurLexem.content == s) {
            GetLexem();
            return true;
        }
    return false;
}

void VariableCreation() {
    Type();
    Name();
    if (CurLexem.content == "=") {
        GetLexem();
        Expression();
    }
    while (CurLexem.content == ",") {
        GetLexem();
        Name();
        if (CurLexem.content == "=") {
            GetLexem();
            Expression();
        }
    }
    if (CurLexem.content != ";")
        throw InvalidVariableCreation();
    GetLexem();
}

bool Sign() {
    if (CurLexem.content == "+" || CurLexem.content == "-") {
        GetLexem();
        return true;
    }
    return false;
}

bool SignedNumber() {
    if (!Sign()) {
        return UnsignedNumber();
    }
    else {
        GetLexem();
        return UnsignedNumber();
    }
}

bool UnsignedNumber() {
    for (char c : CurLexem.content) {
        if (!Digit(c))
            return false;
    }
    GetLexem();
    return true;
}

// Expression

void Expression() {
    ExpressionTerm();
    while (CurLexem.content == ",") {
        GetLexem();
        ExpressionTerm();
    }
}

void ExpressionTerm() {
    ArithmeticExpression();
}

// Arithmetic, logic and comparison

void ArithmeticExpression() {
    UnaryTerm();
}

bool Unary() {
    if (CurLexem.content == "+" || CurLexem.content == "-" || CurLexem.content == "++"
        || CurLexem.content == "--" || CurLexem.content == "~") {
        GetLexem();
        return true;
    }
    return false;
}

bool Mul() {
    if (CurLexem.content == "*" || CurLexem.content == "/" || CurLexem.content == "%") {
        GetLexem();
        return true;
    }
    return false;
}

bool Sum() {
    if (CurLexem.content == "+" || CurLexem.content == "-") {
        GetLexem();
        return true;
    }
    return false;
}

bool Power() {
    if (CurLexem.content == "**") {
        GetLexem();
        return true;
    }
    return false;
}

bool And() {
    if (CurLexem.content == "&" || CurLexem.content == "and") {
        GetLexem();
        return true;
    }
    return false;
}

bool Xor() {
    if (CurLexem.content == "^") {
        GetLexem();
        return true;
    }
    return false;
}

bool Or() {
    if (CurLexem.content == "|" || CurLexem.content == "or") {
        GetLexem();
        return true;
    }
    return false;
}

bool Shift() {
    if (CurLexem.content == "<<" || CurLexem.content == ">>") {
        GetLexem();
        return true;
    }
    return false;
}

bool Equality()
{
    if (CurLexem.content == "==" || CurLexem.content == "!=") {
        GetLexem();
        return true;
    }
    return false;
}

bool NonEquality()
{
    if (CurLexem.content == "<" || CurLexem.content == ">" ||
        CurLexem.content == "<=" || CurLexem.content == ">=") {
        GetLexem();
        return true;
    }
    return false;
}

bool Assignment()
{
    if (CurLexem.content == "=" || CurLexem.content == "<<=" ||
        CurLexem.content == ">>=" || CurLexem.content == "+=" ||
        CurLexem.content == "-=" || CurLexem.content == "*=" ||
        CurLexem.content == "**=" || CurLexem.content == "/=" ||
        CurLexem.content == "//=" || CurLexem.content == "^=" ||
        CurLexem.content == "&=" || CurLexem.content == "|=" ||
        CurLexem.content == "%=") {
        GetLexem();
        return true;
    }
    return false;
}

void UnaryTerm() {
    if (Sign()) {
        UnaryTerm();
    }
    else {
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

void NonEqualityTerm()
{
    EqualityTerm();
    while (NonEquality()) {
        EqualityTerm();
    }
}

void EqualityTerm()
{
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

void AssignmentTerm()
{
    ArithmeticTerm();
    while (Assignment()) {
        ArithmeticTerm();
    }
}

void ArithmeticTerm() {
    if (CurLexem.content == "(") {
        GetLexem();
        ArithmeticExpression();
        if (CurLexem.content != ")")
            throw InvalidArithmeticTerm();
        return;
    }
    if (!BooleanLiteral() && !ArithmeticLiteral()) {
        Name();
        if (CurLexem.content == "(") {
            ArgumentList();
            return;
        }
        if (CurLexem.content == ".") {
            GetLexem();
            Name();
            if (CurLexem.content == "(") {
                ArgumentList();
            }
            return;
        }
        if (CurLexem.content == "[") {
            Expression();
            if (CurLexem.content != "]")
                throw InvalidArrayIndexation();
            GetLexem();
            return;
        }
    }
}

bool BooleanLiteral() {
    if (CurLexem.content == "true" || CurLexem.content == "false") {
        GetLexem();
        return true;
    }
    return false;
}

bool ArithmeticLiteral() {
    if (!SignedNumber())
        return false;
    if (CurLexem.content == ".") {
        GetLexem();
        if (!UnsignedNumber())
            throw InvalidArithmeticLiteral();
    }
    return true;
}
