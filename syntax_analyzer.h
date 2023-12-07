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
void Expression_Operator();
void Variable_Creation();
void Match();
void Variable_Initialization();
void Name();