#pragma once

#include <deque>
#include <string>
#include <vector>

#include "lexical_analyzer.cpp"
#include "CompileError.cpp"
#include "tids.cpp"
// TIDS

const std::string NOT_A_TYPE = "";
const std::string NOT_A_STRUCT = "";

void ReadFile(std::string file, std::vector<char>& text);

std::vector<std::string> TypeList = { "int", "void", "bool", "double" };

// Get lexems

void GetLexem();

// Program Structure

size_t Program();
void Directive();

// Functions

void FunctionDefinition(std::string str = NOT_A_STRUCT);
void FunctionParameters(std::string str = NOT_A_STRUCT);

// Structures

void StructDefinition();

// Types and variables

void String();
void Symbol(char symbol);
std::string Name();
bool Letter(char c);
bool SpecialSymbol(char c);
bool Digit(char c);
std::string Type();
void EntityCreation(std::string str = NOT_A_STRUCT, std::string type = "");
bool VariableCreation(std::string str = NOT_A_STRUCT);

// void VariableDefinition();
//
// void VariableInitialization();

// bool Sign();
// bool SignedNumber();
// bool UnsignedNumber();

// Expression

void Expression();
void ExpressionTerm();

// Arithmetic, logic, comparison and assignment

void ArithmeticExpression();
bool FunctionCall();
bool ArrayAccess();
bool MemberAccess();
bool Unary();
bool Mul();
bool Sum();
bool And();
bool Xor();
bool Or();
bool Shift();
bool Equality();
bool NonEquality();
bool Assignment();
void ArrayFuncMemberTerm();
void UnaryTerm();
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
void OutputItem();

// Arrays

void ArrayDeclaration(const std::string& str);
void ArrayDeclarationAuto(const std::string& str);
void ArrayDeclarationExact(const std::string& str);
void Literal();

// Match

void Match();

void reset_variable_in_creation();
void reset_function_in_creation();
void reset_struct_in_creation();
void reset_variable_in_creation_var();
