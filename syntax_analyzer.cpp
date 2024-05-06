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
        } else if ((tmp = Type()) != NOT_A_TYPE) {
            VariableCreation("", tmp);
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
    // Poliz done
    if (lexem.content != "fun") throw InvalidFunctionDefinition();
    GetLexem();
    IdTIDS.create_TID();
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
    push_stack(poliz_element(ELEMENT_TYPE::CREATE_TID));
    if (lexem.content != "{") throw InvalidFunctionDefinition();
    GetLexem();
    
    while (lexem.content != "}") {
        Operator();
    }
    push_stack(poliz_element(ELEMENT_TYPE::END_OF_FUNCTION));
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
    IdTIDS.cur_tid()->push_id(variable_in_creation);
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
        IdTIDS.cur_tid()->push_id(variable_in_creation);
        reset_variable_in_creation();
    }
    if (lexem.content != ")") throw UndefinedFunctionParameters();
    GetLexem();
}

// void FunctionCall(std::string str) {
//     function_in_creation = Name();
//     if (str == NOT_A_STRUCT)
//         FunTIDS.check_func_id(function_in_creation);
//     else
//         StrTIDS.check_func_id(str, function_in_creation);
//     ArgumentList(str);
//     reset_function_in_creation();
// }

// void ArgumentList(std::string str, std::string name_of_func) {
//     if (lexem.content != "(") throw InvalidArgumentList();
//     GetLexem();
//     if (lexem.content == ")") {
//         GetLexem();
//         return;
//     }
//     int par_count = 0;
//     ExpressionTerm();
//     if (str == NOT_A_STRUCT) {
//         if (function_in_creation == "") function_in_creation = name_of_func;
//         FunTIDS.check_func_par(function_in_creation, par_count++, st.top());
//     } else
//         StrTIDS.check_func_par(str, function_in_creation, par_count++,
//                                st.top());
//     stack_del();
//     while (lexem.content == ",") {
//         GetLexem();
//         ExpressionTerm();
//         if (str == NOT_A_STRUCT)
//             FunTIDS.check_func_par(function_in_creation, par_count++, st.top());
//         else
//             StrTIDS.check_func_par(str, function_in_creation, par_count++,
//                                    st.top());
//         stack_del();
//     }
//     if (str == NOT_A_STRUCT)
//         FunTIDS.check_param_count(function_in_creation, par_count);
//     else
//         StrTIDS.check_param_count(str, function_in_creation, par_count);
//     if (lexem.content != ")") throw InvalidArgumentList();
//     GetLexem();
// }

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
        std::string type = lexem.content;
        bool ok = false;
        for (const std::string& t : TypeList) {
            if (type == t) {
                ok = true;
                break;
            }
        }
        if (ok) {
            VariableCreation(struct_in_creation);
        } else {
            break;
        }
    }
    if (lexem.content != "}") throw InvalidStructDefinition();
    GetLexem();
    if (lexem.content == ";") {
        GetLexem();
        return;
    }
    IdTIDS.del_TID();
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

// void StructMember() {
//     std::string mem = Name();
//     struct_in_creation = "";
//     std::string object;
//     for (int ind = 0; ind < (int)mem.size(); ++ind) {
//         if (ind == '.') {
//             struct_in_creation = mem.substr(0, ind);
//             object = mem.substr(ind + 1);
//             break;
//         }
//     }
//     if (lexem.content == "(") {
//         function_in_creation = object;
//         StrTIDS.check_func_id(struct_in_creation, function_in_creation);
//         ArgumentList(struct_in_creation);
//     }
//     variable_in_creation.set_name(object);
//     StrTIDS.check_id(struct_in_creation, variable_in_creation.name());
// }

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
    std::string str = Name();
    push_stack(poliz_element(ELEMENT_TYPE::VARIABLE_OPERAND, str));
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

void EntityCreation(std::string str, std::string type_) {
    std::string type = variable_in_creation.get_type();
    if (type == "" && (type = Type()) == NOT_A_TYPE) {
        StrTIDS.check_struct_id(type);
    }
    if (type == "") type = type_;
    std::string name = Name();
    variable_in_creation.set_type(type);
    variable_in_creation.set_name(name);

    if (lexem.content == "[") {
        GetLexem();
        variable_in_creation.set_type(variable_in_creation.type() + "_array");
        type += "_array";
        IdTIDS.cur_tid()->push_id(variable_in_creation);
        if (str != NOT_A_TYPE) StrTIDS.push_id(str, variable_in_creation);
        reset_variable_in_creation();

        push_stack(poliz_element(ELEMENT_TYPE::OPERAND, type));
        push_stack(poliz_element(ELEMENT_TYPE::OPERAND, name));

        ArrayDeclaration();

        return;
    }
    push_stack(poliz_element(ELEMENT_TYPE::OPERAND, type));
    push_stack(poliz_element(ELEMENT_TYPE::OPERAND, name));
    push_stack(poliz_element(ELEMENT_TYPE::MAKE_VARIABLE));
    IdTIDS.cur_tid()->push_id(variable_in_creation);

    if (str != NOT_A_TYPE) {
        StrTIDS.push_id(str, variable_in_creation);
    }
    reset_variable_in_creation_var();

    if (lexem.content == "=") {
        push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, "("));
        push_stack(poliz_element(ELEMENT_TYPE::VARIABLE_OPERAND, name));
        push_stack(poliz_element(ELEMENT_TYPE::OPERATOR, "="));
        GetLexem();
        ExpressionTerm();
        push_remaining_elements_to_stack();
        push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, ")"));
        
        return;
    }
}

void VariableCreation(std::string str, std::string type) {
    // POLIZ DONE - i think
    EntityCreation(str, type);
    while (lexem.content == ",") {
        GetLexem();
        EntityCreation(str, type);
    }
    if (lexem.content != ";") throw InvalidVariableCreation();
    reset_variable_in_creation();
    GetLexem();
}

// bool Sign() {
//     if (lexem.content == "+" || lexem.content == "-") {
//         GetLexem();
//         return true;
//     }
//     return false;
// }

// bool SignedNumber() {
//     if (!Sign()) {
//         return UnsignedNumber();
//     } else {
//         return UnsignedNumber();
//     }
// }

// bool UnsignedNumber() {
//     for (char c : lexem.content) {
//         if (!Digit(c)) return false;
//     }
//     GetLexem();
//     return true;
// }

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
    push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, "("));
    ArithmeticExpression();
    if (!poliz.empty() && poliz.back().type_info == UNDEFINED) {
        poliz.back().type_info = VARIABLE_OPERAND;
        IdTIDS.cur_tid()->check_id(poliz.back().maker);
    }
    push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, ")"));
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
        lexem.content == "**=" || lexem.content == "/=" ||
        lexem.content == "//=" || lexem.content == "^=" ||
        lexem.content == "&=" || lexem.content == "|=" ||
        lexem.content == "%=") {
        push_opstack({STACK_ELEMENT_TYPE::OPERATOR_S, lexem.content, ""});
        GetLexem();
        return true;
    }
    return false;
}

void ArrayFuncTerm()
{
    ArithmeticTerm();
    if (ArrayAccess()) {
        check_array_access();
    } else if (FunctionCall()) {
        check_function_call();
    }
}

void MemberTerm()
{
    ArrayFuncTerm();
    while (MemberAccess()) {
        ArrayFuncTerm();
        check_member_access();
    }
}

void UnaryTerm()
{
    if (Unary()) {
        UnaryTerm();
        check_uno();
    } else {
        MemberTerm();
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
        push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, "("));
        GetLexem();
        Expression();
        if (lexem.content != ")") throw InvalidArithmeticTerm();
        GetLexem();
        push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, ")"));
        return;
    }
    if (!BooleanLiteral() && !ArithmeticLiteral()) {
        
        if (lexem.type == identifier_type) {
            push_opstack({STACK_ELEMENT_TYPE::IDENTIFIER_S, "id", lexem.content});
            GetLexem();
            // if (lexem.content == "(") {
            //     // FUNCTION - POLIZ DONE
            //     type = FunTIDS.check_func_id(tmp);
            //     push_stack(poliz_element(ELEMENT_TYPE::FUNCTION_NAME, tmp));
            //     push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, "("));
            //     ArgumentList("", tmp);
            //     push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, ")"));
            //     reset_function_in_creation();
            //     push_typeof({get_basic_type(type), ""});
            // } else if (lexem.content == "[") {
            //     // ARRAY - POLIZ DONE
            //     type = IdTIDS.cur_tid()->check_id(tmp);
            //     if (type.find('_') == -1) throw InvalidName();
            //     type = type.substr(0, type.find('_'));
            //     push_stack(poliz_element(ELEMENT_TYPE::OPERAND, tmp));
            //     GetLexem();
            //     Expression();
            //     eq_int();
            //     if (lexem.content != "]") throw InvalidArrayIndexation();
            //     GetLexem();
            //     push_stack(poliz_element(ELEMENT_TYPE::OPERATOR, "[]"));
            //     push_typeop({get_basic_type(type), tmp + "[]"});
            // } else {
            //     // ONLY VARIABLE - POLIZ DONE
            //     type = IdTIDS.cur_tid()->check_id(tmp);
            //     push_stack(poliz_element(ELEMENT_TYPE::VARIABLE_OPERAND, tmp));
            //     push_typeop({get_basic_type(type), tmp});
            // }
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
        else
            push_opstack({STACK_ELEMENT_TYPE::RVALUE_S, "int", lexem.content});
        GetLexem();
        return true;
    }
    return false;
}

void Operator() {
    if (lexem.content == "{") {
        IdTIDS.create_TID();
        push_stack(poliz_element(ELEMENT_TYPE::CREATE_TID));
        GetLexem();
        while (lexem.content != "}") {
            Operator();
        }
        IdTIDS.del_TID();
        push_stack(poliz_element(ELEMENT_TYPE::DELETE_TID));
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
                if (lexem.content != ";") throw MissingSemicolumn();
            }
            push_stack(poliz_element(ELEMENT_TYPE::RETURN_OPERATOR));
            GetLexem();
            return;
        }
        bool ok = false;
        for (const std::string& t : TypeList) {
            if (t == lexem.content) {
                ok = true;
                break;
            }
        }
        if (ok) {
            VariableCreation();
            return;
        }
        Expression();
        push_stack(poliz_element(ELEMENT_TYPE::CLEAR_STACK));
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

    size_t p1 = cur_ptr();
    poliz_blank();
    push_stack(poliz_element(ELEMENT_TYPE::MOVE_BY_FALSE));

    stack_clear();
    if (lexem.content != ")") {
        throw ExpectedCloseParenthesis();
    }
    GetLexem();
    
    Operator();

    size_t p2 = cur_ptr();
    poliz_blank();
    push_stack(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));

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
    push_stack(poliz_element(ELEMENT_TYPE::CREATE_TID));
    GetLexem();
    VariableCreation();

    size_t p3 = cur_ptr();
    Expression();
    eq_bool();
    stack_clear();
    size_t p1 = cur_ptr();
    poliz_blank();
    push_stack(poliz_element(ELEMENT_TYPE::MOVE_BY_FALSE));
    size_t p2 = cur_ptr();
    poliz_blank();
    push_stack(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));
    if (lexem.content != ";") {
        throw MissingSemicolumn();
    }
    
    size_t p4 = cur_ptr();
    GetLexem();
    Expression();
    stack_clear();
    
    push_stack(p3);
    push_stack(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));
    push_stack(poliz_element(ELEMENT_TYPE::OPERATOR, ";"));
    poliz[p2] = cur_ptr();
    if (lexem.content != ")") {
        throw ExpectedCloseParenthesis();
    }
    GetLexem();
    Operator();
    
    
    push_stack(p4);
    push_stack(ELEMENT_TYPE::UNCOND_MOVE);
    poliz[p1] = cur_ptr();
    IdTIDS.del_TID();
    push_stack(poliz_element(ELEMENT_TYPE::DELETE_TID));
}

void While() {
    if (lexem.content != "(") {
        throw ExpectedOpenParenthesis();
    }
    size_t p1 = cur_ptr();
    GetLexem();
    Expression();
    eq_bool();
    stack_clear();

    size_t p2 = cur_ptr();
    poliz_blank();
    push_stack(poliz_element(MOVE_BY_FALSE));

    if (lexem.content != ")") {
        throw ExpectedCloseParenthesis();
    }

    GetLexem();
    Operator();
    push_stack(poliz_element(p1));
    push_stack(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));
    poliz[p2] = cur_ptr();
}

void Input() {
    std::string tmp = lexem.content;
    push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, "("));
    Variable();
    IdTIDS.cur_tid()->check_id(tmp);
    size_t count_operands = 1;
    while (lexem.content == ",") {
        GetLexem();
        Variable();
        ++count_operands;
    }
    if (lexem.content != ";") {
        throw MissingSemicolumn();
    }
    push_remaining_elements_to_stack();
    push_stack(poliz_element(ELEMENT_TYPE::LITERAL_OPERAND, std::to_string(literal_int_type) + "_" + std::to_string(count_operands)));
    push_stack(poliz_element(ELEMENT_TYPE::OPERATOR, "input"));
    push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, ")"));
    GetLexem();
}

void Output() {
    
    size_t count_operands = 1;
    push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, "("));
    OutputItem();
    while (lexem.content == ",") {
        GetLexem();
        OutputItem();
        ++count_operands;
    }
    
    if (lexem.content != ";") {
        throw MissingSemicolumn();
    }
    
    push_remaining_elements_to_stack();
    push_stack(poliz_element(ELEMENT_TYPE::LITERAL_OPERAND, std::to_string(literal_int_type) + "_" + std::to_string(count_operands)));
    push_stack(poliz_element(ELEMENT_TYPE::OPERATOR, "output"));
    push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, ")"));
    GetLexem();
}

void OutputItem() {
    if (lexem.type == string_literal_type) {
        push_stack(poliz_element(ELEMENT_TYPE::LITERAL_OPERAND, std::to_string(lexem.type) + "_" + lexem.content));
        GetLexem();
        return;
    }
    // if (lexem.content[0] == '\"' && lexem.content.back() == '\"') {
    //     String();
    //     return;
    // }
    if (lexem.content[0] == '\"') throw InvalidStringLiteral();
    ExpressionTerm();
}

void Match() {
    Expression();
    if (lexem.content != "{") {
        throw ExpectedFigureOpen();
    }
    push_stack(poliz_element(ELEMENT_TYPE::REM_MATCH_MAIN_TERM));
    GetLexem();
    size_t p = cur_ptr();
    poliz_blank();
    push_stack(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));

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
        push_stack(poliz_element(ELEMENT_TYPE::UNCOND_MOVE));
    }
    poliz[p] = cur_ptr();
    for (int i = 0; i < (int)pts.size(); ++i) {
        push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, "("));
        push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, "("));
        push_stack(poliz_element(ELEMENT_TYPE::GET_MATCH_MAIN_TERM));
        push_stack(poliz_element(ELEMENT_TYPE::LITERAL_OPERAND, 
            std::to_string(case_literals[i].type) + "_" + case_literals[i].content));
        push_stack(poliz_element(ELEMENT_TYPE::OPERATOR, "=="));
        push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, ")"));
        push_stack(poliz_element(pts[i]));
        push_stack(poliz_element(ELEMENT_TYPE::PARENTHESIS, ")"));
        push_stack(poliz_element(ELEMENT_TYPE::MOVE_BY_TRUE));
        
    }
    for (size_t x : to_fill) {
        poliz[x] = cur_ptr();
    }
    GetLexem();
}

void ArrayDeclaration() {
    if (lexem.content == "]") {
        GetLexem();
        ArrayDeclarationAuto();
        push_stack(poliz_element(ELEMENT_TYPE::MAKE_ARRAY_AUTO));
    } else {
        ArrayDeclarationExact();
        push_stack(poliz_element(ELEMENT_TYPE::MAKE_ARRAY_EXACT));
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

// is not used anywhere
/*
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
    push_stack(poliz_element(ELEMENT_TYPE::GET_ARRAY_ELEMENT));
    GetLexem();
}
*/

void Literal() {
    // Poliz done
    if (lexem.content == "false" || lexem.content == "true") {
        BooleanLiteral();
    } else {
        ArithmeticLiteral();
    }
}
