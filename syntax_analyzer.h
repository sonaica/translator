#pragma once
#include "SyntaxError.h"

#include <deque>
#include <string>
#include <vector>

#include "lexical_analyzer.h"

void ReadFile(std::string file, std::vector<char> &text);

const std::vector<std::string> TypeList = {"int", "void", "bool", "double"};

// Get lexems

void GetLexem();

// Program Structure

void Program();
void Directive();

// Functions

void FunctionDefinition();
void FunctionParameters();
void FunctionCall();
void ArgumentList();

// Structures

void StructDefinition();
void StructMember();

// Types and variables

void String();
void Symbol(char symbol);
void Name();
void Variable();
bool Letter(char c);
bool SpecialSymbol(char c);
bool Digit(char c);
bool Type();
void VariableCreation();

// void VariableDefinition();
//
// void VariableInitialization();

bool Sign();
bool SignedNumber();
bool UnsignedNumber();

// Expression

void Expression();
void ExpressionTerm();

// Arithmetic, logic, comparison and assignment

void ArithmeticExpression();
bool Unary();
bool Mul();
bool Sum();
bool Power();
bool And();
bool Xor();
bool Or();
bool Shift();
bool Equality();
bool NonEquality();
bool Assignment();
void UnaryTerm();
void PowerTerm();
void MulTerm();
void SumTerm();
void ShiftTerm();
void NonEqualityTerm();
void EqualityTerm();
void AndTerm();
void XorTerm();
void OrTerm();
void AssignmentTerm();
void ArithmeticTerm();
bool ArithmeticLiteral();
bool BooleanLiteral();

// Operator

void Operator();

// Conditional operator

void If();

// Cycles

void For();
void While();

// Input and Output

void Input();
void Output();

// Arrays

void ArrayIndexation();
void ArrayDeclaration();
void ArrayDeclarationAuto();
void ArrayDeclarationExact();
void Literal();

// Match

void Match();
