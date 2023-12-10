#pragma once
#include <vector>
#include <string>
#include "lexical_analyzer.h"

void ReadFile(std::string file, std::vector<char> &text);
void Program();
Lexem request_lexem();

void Assignment();
void Variable();
void Assignment_Operator();
void Expression();
void Comparison();
void Comparisons_Operators();
void Operator();
void If();
void For();
void While();
void Input();
void Output();
void Variable_Creation();
void Match();
void Variable_Initialization();
void Name();
void Array_Declaration();
void Array_Declaration_Exact();
void Array_Declaration_Auto();
void Type();
void Literal();
void Array_Indexation();
void Arithmetic_Literal();
void Boolean_Literal();