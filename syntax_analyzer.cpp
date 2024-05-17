#pragma once
#include "syntax_analyzer.h"
#include "poliz.cpp"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>

#include "CompileError.cpp"
#include "lexical_analyzer.cpp"
#include "operation_stack.cpp"
#include "global_tids.hpp"
extern std::vector<char> text;
extern int pos;
extern Lexem lexem;

Verdict verdict;

extern IdentifierTIDS IdTIDS;
extern FunctionTIDS FunTIDS;
extern StructTIDS StrTIDS;

extern std::string function_in_creation;
extern std::string struct_in_creation;
extern std::string current_type;
extern Value variable_in_creation;

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
    while (!text.empty() && text.back() != '}')
        text.pop_back();
    if (text.empty()) {
        std::cout << "We don't compile void here";
        exit(0);
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

void reset_variable_in_creation_var() { variable_in_creation.name_ = ""; }

void reset_function_in_creation() { function_in_creation = ""; }

void reset_struct_in_creation() { struct_in_creation = ""; }

// Program Structure

size_t Program() {
    for (;;) {
        if (lexem.content == "import" || lexem.content == "define")
            Directive();
        else
            break;
    }
    for (;;) {
        std::string tmp;
        if (lexem.content == "fun") {
            FunctionDefinition();
        } else if (lexem.content == "struct") {
            StructDefinition();
        } else if (!VariableCreation()) {
            break;
        }
    }
    if (lexem.content != "main") throw UndefinedMainFunctionError();
    GetLexem();
    if (lexem.content != "(") throw UndefinedMainFunctionError();
    GetLexem();
    if (lexem.content != ")") throw UndefinedMainFunctionError();
    GetLexem();
    size_t pos = cur_ptr();
    Operator();
    while (!IdTIDS.is_empty())
        IdTIDS.del_TID();
    return pos;
}

void Directive() {
    if (lexem.content == "import") {
        GetLexem();
        if (lexem.type != string_literal_type) throw InvalidFilename();
        GetLexem();
        if (lexem.content != ";") throw MissingSemicolumn();
        stack_clear();
        GetLexem();
        return;
    }
    if (lexem.content == "define") {
        GetLexem();
        String();
        String();
        if (lexem.content != ";") throw MissingSemicolumn();
        stack_clear();
        GetLexem();
        return;
    }
}

// Functions

void FunctionDefinition(std::string str) {
    // Poliz done
    if (lexem.content != "fun") throw InvalidFunctionDefinition();
    GetLexem();
    IdTIDS.create_TID();
    push_poliz(poliz_element(ELEMENT_TYPE::CREATE_TID));
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
    if (str == NOT_A_STRUCT) {
        FunTIDS.push_func_return_type(function_in_creation, current_type);
        FunTIDS.push_func_poliz_pos(function_in_creation, cur_ptr());
    } else {
        StrTIDS.push_func_return_type(str, function_in_creation, current_type);
        StrTIDS.push_func_poliz_pos(str, function_in_creation, cur_ptr());
    }
    
    if (lexem.content != "{") throw InvalidFunctionDefinition();
    GetLexem();
    
    while (lexem.content != "}") {
        Operator();
    }
    push_poliz(poliz_element(ELEMENT_TYPE::END_OF_FUNCTION));
    IdTIDS.del_TID();
    // IdTID of function will be deleted in ELEMENT_TYPE::RETURN_OPERATOR or ELEMENT_TYPE::END_OF_FUNCTION
    GetLexem();
    reset_function_in_creation();
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
    variable_in_creation.set_name(Name());
    if (str == NOT_A_STRUCT)
        FunTIDS.push_func_par(function_in_creation, variable_in_creation);
    else
        StrTIDS.push_func_par(str, function_in_creation, variable_in_creation);
    IdTIDS.cur_tid()->push_id(variable_in_creation, StrTIDS);
    reset_variable_in_creation();
    while (lexem.content == ",") {
        GetLexem();
        if ((current_type = Type()) == NOT_A_TYPE) {
            current_type = Name();
            StrTIDS.check_struct_id(current_type);
        }
        variable_in_creation.set_type(current_type);
        variable_in_creation.set_name(Name());
        if (str == NOT_A_STRUCT)
            FunTIDS.push_func_par(function_in_creation, variable_in_creation);
        else
            StrTIDS.push_func_par(str, function_in_creation,
                                  variable_in_creation);
        IdTIDS.cur_tid()->push_id(variable_in_creation, StrTIDS);
        reset_variable_in_creation();
    }
    if (lexem.content != ")") throw UndefinedFunctionParameters();
    GetLexem();
}

// Structures

void StructDefinition() {
    if (lexem.content != "struct") throw InvalidStructDefinition();
    GetLexem();
    struct_in_creation = Name();
    TypeList.push_back(struct_in_creation);
    StrTIDS.push_struct_id(struct_in_creation);
    if (lexem.content != "{") throw InvalidStructDefinition();
    GetLexem();
    IdTIDS.create_TID();

    for (;;) {
        if (lexem.content == "fun") {
            FunctionDefinition(struct_in_creation);
            continue;
        }
        if (!VariableCreation(struct_in_creation)) {
            break;
        }
    }
    if (lexem.content != "}") throw InvalidStructDefinition();
    GetLexem();
    IdTIDS.del_TID();
    if (lexem.content == ";") {
        stack_clear();
        GetLexem();
        return;
    }
    
    variable_in_creation.set_name(Name());
    variable_in_creation.set_type(struct_in_creation);
    IdTIDS.cur_tid()->push_id(
        Value(struct_in_creation, variable_in_creation.name()), StrTIDS);
    push_poliz({ELEMENT_TYPE::OPERAND, variable_in_creation.type()});
    push_poliz({ELEMENT_TYPE::OPERAND, variable_in_creation.name()});
    push_poliz({ELEMENT_TYPE::MAKE_VARIABLE, ""});
    while (lexem.content == ",") {
        GetLexem();
        variable_in_creation.set_name(Name());
        IdTIDS.cur_tid()->push_id(
            Value(struct_in_creation, variable_in_creation.name()), StrTIDS);
        push_poliz({ELEMENT_TYPE::OPERAND, variable_in_creation.type()});
        push_poliz({ELEMENT_TYPE::OPERAND, variable_in_creation.name()});
        push_poliz({ELEMENT_TYPE::MAKE_VARIABLE, ""});
    }
    if (lexem.content != ";") throw InvalidStructDefinition();
    stack_clear();
    GetLexem();
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
    if (lexem.type != identifier_type) {
        throw InvalidName();
    }
    std::string name = lexem.content;
    GetLexem();
    return name;
}

std::string Type() {
    for (const std::string& s : TypeList)
        if (lexem.content == s) {
            GetLexem();
            return s;
        }
    return NOT_A_TYPE;
}

void EntityCreation(std::string str, std::string type) {
    std::string name = Name();
    variable_in_creation.set_type(type);
    variable_in_creation.set_name(name);
    if (lexem.content == "[") {
        // CHECK THIS LATER
        GetLexem();
        variable_in_creation.set_type(variable_in_creation.type() + "_array");
        push_poliz(poliz_element(ELEMENT_TYPE::OPERAND, type));
        push_poliz(poliz_element(ELEMENT_TYPE::OPERAND, name));
        ArrayDeclaration(str);

        reset_variable_in_creation();

        return;
    }
    push_poliz(poliz_element(ELEMENT_TYPE::OPERAND, type));
    push_poliz(poliz_element(ELEMENT_TYPE::OPERAND, name));
    push_poliz(poliz_element(ELEMENT_TYPE::MAKE_VARIABLE));
    IdTIDS.cur_tid()->push_id(variable_in_creation, StrTIDS);
    if (str != NOT_A_TYPE) {
        StrTIDS.push_id(str, variable_in_creation);
    }
    reset_variable_in_creation_var();

    if (lexem.content == "=") {
        push_poliz(poliz_element(ELEMENT_TYPE::VARIABLE_OPERAND, name));
        GetLexem();
        ExpressionTerm();
        check_left();
        push_poliz(poliz_element(ELEMENT_TYPE::OPERATOR, "="));
        
        return;
    }
}

bool VariableCreation(std::string str) {
    // POLIZ DONE - i think
    std::string type = Type();
    if (type == NOT_A_TYPE)
        return false;
    EntityCreation(str, type);
    while (lexem.content == ",") {
        GetLexem();
        EntityCreation(str, type);
    }
    if (lexem.content != ";") throw InvalidVariableCreation();
    stack_clear();
    reset_variable_in_creation();
    GetLexem();
    return true;
}

// Expression

void Expression() {
    // POLIZ DONE - i think
    ExpressionTerm();
    while (lexem.content == ",") {
        GetLexem();
        ExpressionTerm();
    }
}

// EXPRESSION TERM - POLIZ DONE
void ExpressionTerm() { 
    ArithmeticExpression();
}

// Arithmetic, logic and comparison

void ArithmeticExpression() { AssignmentTerm(); }

bool FunctionCall()
{
    if (lexem.content == "(") {
        GetLexem();
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, "()", ""});
        
        if (lexem.content == ")") {
            GetLexem();
            return true;
        }
        Expression();
        
        if (lexem.content != ")") {
            throw InvalidFunctionCall();
        }
        GetLexem();
        
        return true;
    }
    return false;
}

bool ArrayAccess()
{
    if (lexem.content == "[") {
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, "[]", ""});
        GetLexem();
        ExpressionTerm();
        if (lexem.content != "]") {
            throw InvalidArrayIndexation();
        }
        GetLexem();
        return true;
    }
    return false;
}

bool MemberAccess()
{
    if (lexem.content == ".") {
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, ".", ""});
        GetLexem();
        return true;
    }
    return false;
}

bool Unary() {
    // Poliz done
    if (lexem.content == "+" || lexem.content == "-" || lexem.content == "++" ||
        lexem.content == "--" || lexem.content == "~") {
        std::string op = lexem.content;
        if (op == "-" || op == "+")
            op += 'u';
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, op, ""});
        GetLexem();
        return true;
    }
    return false;
}

bool Mul() {
    // Poliz done
    if (lexem.content == "*" || lexem.content == "/" || lexem.content == "%") {
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, lexem.content, ""});
        GetLexem();
        return true;
    }
    return false;
}

bool Sum() {
    // Poliz done
    if (lexem.content == "+" || lexem.content == "-") {
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, lexem.content, ""});
        GetLexem();
        return true;
    }
    return false;
}

bool And() {
    // Poliz done
    if (lexem.content == "&" || lexem.content == "and") {
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, lexem.content, ""});
        GetLexem();
        return true;
    }
    return false;
}

bool Xor() {
    // Poliz done
    if (lexem.content == "^") {
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, lexem.content, ""});
        GetLexem();
        return true;
    }
    return false;
}

bool Or() {
    // Poliz done
    if (lexem.content == "|" || lexem.content == "or") {
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, lexem.content, ""});
        GetLexem();
        return true;
    }
    return false;
}

bool Shift() {
    // Poliz done
    if (lexem.content == "<<" || lexem.content == ">>") {
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, lexem.content, ""});
        GetLexem();
        return true;
    }
    return false;
}

bool Equality() {
    // Poliz done
    if (lexem.content == "==" || lexem.content == "!=") {
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, lexem.content, ""});
        GetLexem();
        return true;
    }
    return false;
}

bool NonEquality() {
    // Poliz done
    if (lexem.content == "<" || lexem.content == ">" || lexem.content == "<=" ||
        lexem.content == ">=") {
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, lexem.content, ""});
        GetLexem();
        return true;
    }
    return false;
}

bool Assignment() {
    // Poliz done
    if (lexem.content == "=" || lexem.content == "<<=" ||
        lexem.content == ">>=" || lexem.content == "+=" ||
        lexem.content == "-=" || lexem.content == "*=" ||
        lexem.content == "%=" || lexem.content == "/=" ||
        lexem.content == "^=" || lexem.content == "&=" || 
        lexem.content == "|=") {
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, lexem.content, ""});
        GetLexem();
        return true;
    }
    return false;
}

void ArrayFuncMemberTerm()
{
    ArithmeticTerm();
    while (lexem.content == "." || lexem.content == "[" || lexem.content == "(") {
        if (MemberAccess()) {
            ArithmeticTerm();
            check_member_access();
        } else if (ArrayAccess()) {
            check_array_access();
        } else if (FunctionCall()) {
            check_function_call();
        }
    }

}

void UnaryTerm()
{
    if (Unary()) {
        UnaryTerm();
        check_uno();
    } else {
        ArrayFuncMemberTerm();
    }
}

void MulTerm() {
    UnaryTerm();
    while (Mul()) {
        UnaryTerm();
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
        check_assignment();
    }
}

void ArithmeticTerm() {
    // POLIZ DONE
    if (lexem.content == "(") {
        GetLexem();
        Expression();
        if (lexem.content != ")") throw InvalidArithmeticTerm();
        GetLexem();
        return;
    }
    if (!BooleanLiteral() && !ArithmeticLiteral()) {
        
        if (lexem.type == identifier_type) {
            push_opstack({STACK_ELEMENT_TYPE::IDENTIFIER_S, "id", lexem.content});
            GetLexem();
        } else {
            throw InvalidName();
        }
    }
}

bool BooleanLiteral() {
    // Poliz done
    if (lexem.type == literal_bool_type) {
        // push_stack(poliz_element(ELEMENT_TYPE::LITERAL_OPERAND, std::to_string(lexem.type) + "_" + lexem.content));
        push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, "bool", lexem.content});
        GetLexem();
        return true;
    }
    return false;
}

bool ArithmeticLiteral() {
    // Poliz Done
    if (lexem.type == literal_int_type || lexem.type == literal_double_type) {
        // push_stack(poliz_element(ELEMENT_TYPE::LITERAL_OPERAND, std::to_string(lexem.type) + "_" + lexem.content));
        if (lexem.type == literal_double_type)
            push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, "double", lexem.content});
        else if (lexem.type == literal_int_type)
            push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, "int", lexem.content});
        GetLexem();
        return true;
    }
    return false;
}

void Operator() {
    if (lexem.content == "{") {
        IdTIDS.create_TID();
        push_poliz(poliz_element(ELEMENT_TYPE::CREATE_TID));
        GetLexem();
        while (lexem.content != "}") {
            Operator();
        }
        IdTIDS.del_TID();
        push_poliz(poliz_element(ELEMENT_TYPE::DELETE_TID));
        GetLexem();
    } else {
        if (lexem.content == "if") {
            // Poliz done
            GetLexem();
            If();
            return;
        }
        if (lexem.content == "while") {
            // Poliz done
            GetLexem();
            While();
            return;
        }
        if (lexem.content == "for") {
            // Poliz done
            GetLexem();
            For();
            return;
        }
        if (lexem.content == "input") {
            // Poliz done (i think)
            GetLexem();
            Input();
            return;
        }
        if (lexem.content == "output") {
            // Poliz done (i think)
            GetLexem();
            Output();
            return;
        }
        if (lexem.content == "match") {
            // Poliz done (i think)
            GetLexem();
            Match();
            return;
        }
        if (lexem.content == "return") {
            // Poliz done
            GetLexem();
            if (lexem.content != ";") {
                Expression();
                check_left();
                if (lexem.content != ";") throw MissingSemicolumn();
            }
            push_poliz(poliz_element(ELEMENT_TYPE::RETURN_OPERATOR));
            GetLexem();
            return;
        }
        if (VariableCreation())
            return;
        Expression();
        check_left();
        push_poliz(poliz_element(ELEMENT_TYPE::CLEAR_STACK));
        if (lexem.content != ";") {
            throw MissingSemicolumn();
        }
        stack_clear();
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

    size_t p1 = cur_ptr();
    poliz_blank();
    push_poliz(poliz_element(ELEMENT_TYPE::MOVE_BY_FALSE));
    stack_clear();

    if (lexem.content != ")") {
        throw ExpectedCloseParenthesis();
    }

    GetLexem(); 
    Operator();

    size_t p2 = cur_ptr();
    poliz_blank();
    push_poliz(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));

    poliz[p1] = cur_ptr();

    if (lexem.content == "else") {
        GetLexem();
        Operator();
    }

    poliz[p2] = cur_ptr();
}

void For() {
    if (lexem.content != "(") {
        throw ExpectedOpenParenthesis();
    }
    IdTIDS.create_TID();
    push_poliz(poliz_element(ELEMENT_TYPE::CREATE_TID));
    GetLexem();
    if (lexem.content != ";")
        VariableCreation();
    else
        GetLexem();

    bool have_cond = false;
    size_t p1, p2, p3, p4;
    p3 = cur_ptr();
    if (lexem.content == ";") {
        GetLexem();
    } else {
        have_cond = true;
        Expression();
        eq_bool();

        p1 = cur_ptr();
        poliz_blank();
        push_poliz(poliz_element(ELEMENT_TYPE::MOVE_BY_FALSE));

        
        if (lexem.content != ";") {
            throw MissingSemicolumn();
        }
        GetLexem();
    }

    p2 = cur_ptr();
    poliz_blank();
    push_poliz(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));
    
    p4 = cur_ptr();
    if (lexem.content == ")") {
        GetLexem();
        push_poliz(p3);
        push_poliz(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));
    } else {
        Expression();
        stack_clear();
        if (lexem.content != ")") {
            throw ExpectedCloseParenthesis();
        }
        GetLexem();
        push_poliz(poliz_element(ELEMENT_TYPE::CLEAR_STACK));
        push_poliz(p3);
        push_poliz(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));
    }
    poliz[p2] = cur_ptr();

    Operator();
    
    
    push_poliz(p4);
    push_poliz(ELEMENT_TYPE::UNCOND_MOVE);
    if (have_cond)
        poliz[p1] = cur_ptr();
    IdTIDS.del_TID();
    push_poliz(poliz_element(ELEMENT_TYPE::DELETE_TID));
}

void While() {
    if (lexem.content != "(") {
        throw ExpectedOpenParenthesis();
    }
    size_t p1 = cur_ptr();
    GetLexem();
    Expression();
    eq_bool();

    size_t p2 = cur_ptr();
    poliz_blank();
    push_poliz(poliz_element(MOVE_BY_FALSE));

    if (lexem.content != ")") {
        throw ExpectedCloseParenthesis();
    }

    GetLexem();
    Operator();
    push_poliz(poliz_element(p1));
    push_poliz(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));
    poliz[p2] = cur_ptr();
}

void Input() {
    push_opstack(stack_element(STACK_ELEMENT_TYPE::INPUT_END_S, "", ""));
    Expression();
    check_input();
    GetLexem();
}

void Output() {
    size_t count_operands = 1;
    push_opstack(stack_element(STACK_ELEMENT_TYPE::OUTPUT_END_S, "", ""));
    OutputItem();
    while (lexem.content == ",") {
        GetLexem();
        OutputItem();
        ++count_operands;
    }
    if (lexem.content != ";") {
        throw MissingSemicolumn();
    }
    check_output();
    GetLexem();
}

void OutputItem() {
    if (lexem.type == string_literal_type) {
        push_opstack(stack_element(STACK_ELEMENT_TYPE::RVALUE_S, "string_literal", lexem.content, true));
        GetLexem();
        return;
    }
    if (lexem.content[0] == '\"') throw InvalidStringLiteral();
    ExpressionTerm();
    check_left();
}

void Match() {
    Expression();
    if (lexem.content != "{") {
        throw ExpectedFigureOpen();
    }
    push_poliz(poliz_element(ELEMENT_TYPE::REM_MATCH_MAIN_TERM));
    GetLexem();
    size_t p = cur_ptr();
    poliz_blank();
    push_poliz(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));

    vector<size_t> pts;
    vector<size_t> to_fill;
    vector<Lexem> case_literals;
    while (lexem.content != "}") {
        case_literals.push_back(lexem);
        Literal();
        stack_clear();
        if (lexem.content != "=>") {
            throw ExpectedMatch();
        }
        pts.push_back(cur_ptr());
        GetLexem();
        Operator();
        to_fill.push_back(cur_ptr());
        poliz_blank();
        push_poliz(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));
    }
    poliz[p] = cur_ptr();
    for (int i = 0; i < (int)pts.size(); ++i) {
        push_poliz(poliz_element(ELEMENT_TYPE::GET_MATCH_MAIN_TERM));
        push_poliz(poliz_element(ELEMENT_TYPE::LITERAL_OPERAND, 
            std::to_string(case_literals[i].type) + "_" + case_literals[i].content));
        push_poliz(poliz_element(ELEMENT_TYPE::OPERATOR, "=="));
        push_poliz(poliz_element(pts[i]));
        push_poliz(poliz_element(ELEMENT_TYPE::MOVE_BY_TRUE));
    }
    push_poliz(poliz_element(ELEMENT_TYPE::DELETE_MATCH_MAIN_TERM));
    for (size_t x : to_fill) {
        poliz[x] = cur_ptr();
    }
    GetLexem();
}

void ArrayDeclaration(const std::string& str) {
    if (lexem.content == "]") {
        GetLexem();
        ArrayDeclarationAuto(str);
        push_poliz(poliz_element(ELEMENT_TYPE::MAKE_ARRAY_AUTO));
    } else {
        ArrayDeclarationExact(str);
        push_poliz(poliz_element(ELEMENT_TYPE::MAKE_ARRAY_EXACT));
    }
}

void ArrayDeclarationAuto(const std::string& str) {
    if (lexem.content != "=") {
        throw ExpectedEqual();
    }
    GetLexem();
    if (lexem.content != "{") {
        throw ExpectedFigureOpen();
    }
    GetLexem();
    Expression();
    IdTIDS.cur_tid()->push_id(variable_in_creation, StrTIDS);
    if (str != NOT_A_TYPE)
        StrTIDS.push_id(str, variable_in_creation);
    if (lexem.content != "}") {
        throw ExpectedFigureClose();
    }
    GetLexem();
}

void ArrayDeclarationExact(const std::string& str) {
    if (lexem.type != literal_int_type)
        throw ExpectedIntegerLiteral();
    push_poliz(poliz_element(ELEMENT_TYPE::LITERAL_OPERAND, std::to_string(literal_int_type) + "_" + lexem.content));
    int cnt = stoi(lexem.content);
    IdTIDS.cur_tid()->push_id(variable_in_creation, StrTIDS, cnt);
    if (str != NOT_A_TYPE)
        StrTIDS.push_id(str, variable_in_creation, cnt);
    GetLexem();
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
        Expression();
        if (lexem.content != "}") {
            throw ExpectedFigureClose();
        }
        GetLexem();
    }
}


void Literal() {
    // Poliz done
    if (lexem.content == "false" || lexem.content == "true") {
        BooleanLiteral();
    } else {
        ArithmeticLiteral();
    }
    check_left();
}
