#pragma once
#include "CompileError.h"

CompileError::CompileError(std::string ErrorText)
    : ErrorText_(ErrorText){}

CompileError::~CompileError() = default;

void CompileError::what() { std::cerr << ErrorText_; }

// ----------------------- SYNTAX ERRORS -----------------------

// Undefined Main Function

UndefinedMainFunctionError::UndefinedMainFunctionError()
    : CompileError(
          "Compile time error: the main function isn't defined, or is defined improperly"){};

UndefinedMainFunctionError::~UndefinedMainFunctionError() = default;

// Invalid Filename

InvalidFilename::InvalidFilename()
    : CompileError("Compile time error: the filename is written improperly") {}

InvalidFilename::~InvalidFilename() = default;

// Undefined Function Parameters

UndefinedFunctionParameters::UndefinedFunctionParameters()
    : CompileError(
          "Compile time error: parameters of the function are undefined, or defined "
          "improperly") {}

UndefinedFunctionParameters::~UndefinedFunctionParameters() = default;

// Invalid Function Call

InvalidArgumentList::InvalidArgumentList()
    : CompileError("Compile time error: argument list of the function is used improperly") {}

InvalidArgumentList::~InvalidArgumentList() = default;

// Invalid Struct Definition

InvalidStructDefinition::InvalidStructDefinition()
    : CompileError("Compile time error: invalid struct definition") {}

InvalidStructDefinition::~InvalidStructDefinition() = default;

// Invalid Function Definition

InvalidFunctionDefinition::InvalidFunctionDefinition()
    : CompileError("Compile time error: invalid function definition") {}

InvalidFunctionDefinition::~InvalidFunctionDefinition() = default;

// Invalid Struct Member

InvalidStructMember::InvalidStructMember()
    : CompileError("Compile time error : struct member is used improperly") {}

InvalidStructMember::~InvalidStructMember() = default;

// Invalid Symbol

InvalidSymbol::InvalidSymbol()
    : CompileError("Compile time error: invalid symbol is used") {}

InvalidSymbol::~InvalidSymbol() = default;

// Invalid Name

InvalidName::InvalidName() : CompileError("Compile time error: invalid name") {}

InvalidName::~InvalidName() = default;

// Invalid Array Indexation

InvalidArrayIndexation::InvalidArrayIndexation()
    : CompileError("Compile time error: array indexation is written improperly") {}

InvalidArrayIndexation::~InvalidArrayIndexation() = default;

// Missing a semicolumn

MissingSemicolumn::MissingSemicolumn()
    : CompileError("Compile time error: missing a \";\""){};

MissingSemicolumn::~MissingSemicolumn() = default;

// Invalid Variable Creation

InvalidVariableCreation::InvalidVariableCreation()
    : CompileError("Compile time error: creation of the variable is written improperly") {}

InvalidVariableCreation::~InvalidVariableCreation() = default;

// Invalid Unsigned Number

InvalidUnsignedNumber::InvalidUnsignedNumber()
    : CompileError("Compile time error: invalid unsigned number") {}

InvalidUnsignedNumber::~InvalidUnsignedNumber() = default;

// Invalid Arithmetic Term

InvalidArithmeticTerm::InvalidArithmeticTerm()
    : CompileError("Compile time error: invalid arithmetic term") {}

InvalidArithmeticTerm::~InvalidArithmeticTerm() = default;

InvalidArithmeticLiteral::InvalidArithmeticLiteral()
    : CompileError("Compile time error: invalid arithmetic literal") {}

InvalidArithmeticLiteral::~InvalidArithmeticLiteral() = default;

ExpectedOpenParenthesis::ExpectedOpenParenthesis()
    : CompileError("Compile time error: expected a \"(\"") {}

ExpectedOpenParenthesis::~ExpectedOpenParenthesis() = default;

ExpectedCloseParenthesis::ExpectedCloseParenthesis()
    : CompileError("Compile time error: expected a \")\""){};

ExpectedCloseParenthesis::~ExpectedCloseParenthesis() = default;

ExpectedDo::ExpectedDo() : CompileError("Compile time error: expected \"do\""){};

ExpectedDo::~ExpectedDo() = default;

ExpectedFigureOpen::ExpectedFigureOpen()
    : CompileError("Compile time error: expected \"{\""){};

ExpectedFigureOpen::~ExpectedFigureOpen() = default;

ExpectedFigureClose::ExpectedFigureClose()
    : CompileError("Compile time error: expected \"}\""){};

ExpectedFigureClose::~ExpectedFigureClose() = default;

ExpectedMatch::ExpectedMatch() : CompileError("Compile time error: expected \">=\""){};

ExpectedMatch::~ExpectedMatch() = default;

ExpectedSquareOpen::ExpectedSquareOpen()
    : CompileError("Compile time error: expected \"[\""){};

ExpectedSquareOpen::~ExpectedSquareOpen() = default;

ExpectedSquareClose::ExpectedSquareClose()
    : CompileError("Compile time error: expected \"]\""){};

ExpectedSquareClose::~ExpectedSquareClose() = default;

ExpectedComma::ExpectedComma() : CompileError("Compile time error: expected \",\""){};

ExpectedComma::~ExpectedComma() = default;

ExpectedEqual::ExpectedEqual() : CompileError("Compile time error: expected \"=\""){};

ExpectedEqual::~ExpectedEqual() = default;

ImpossibleLexem::ImpossibleLexem()
    : CompileError("Compile time error: impossible to allocate a lexem"){};

ImpossibleLexem::~ImpossibleLexem() = default;

InvalidStringLiteral::InvalidStringLiteral()
    : CompileError("Compile time error: invalid string literal"){};

InvalidStringLiteral::~InvalidStringLiteral() = default;

// InvalidInitialization::InvalidInitialization()
//     : CompileError("Compile time error: invalid initialization"){};

// InvalidInitialization::~InvalidInitialization() = default;

// ----------------------- SEMANTIC ERRORS -----------------------

IdentifierAlreadyDefined::IdentifierAlreadyDefined(
    const std::string& VariableName)
    : CompileError("Compile time error: variable with name (" + VariableName +
                   ") was defined more than once") {}

IdentifierAlreadyDefined::~IdentifierAlreadyDefined() = default;

UndefinedVariable::UndefinedVariable(const std::string& VariableName)
    : CompileError("Compile time error: variable with name (" + VariableName +
                   ") is undefined") {}

UndefinedVariable::~UndefinedVariable() = default;

FunctionAlreadyDefined::FunctionAlreadyDefined(const std::string& FunctionName)
    : CompileError("Compile time error: function with name (" + FunctionName +
                   ") was defined more than once") {}

FunctionAlreadyDefined::~FunctionAlreadyDefined() {}

UndefinedFunction::UndefinedFunction(const std::string& FunctionName)
    : CompileError("Compile time error: function with name (" + FunctionName +
                   ") is undefined") {}

UndefinedFunction::~UndefinedFunction() = default;

InvalidParameter::InvalidParameter(const std::string& FunctionName,
                                   const std::string& ExpectedType,
                                   const std::string& GivenType)
    : CompileError("Compile time error: invalid parameter entered in function (" +
                   FunctionName + "). Expected (" + ExpectedType +
                   "), given (" + GivenType + ")") {}

InvalidParameter::~InvalidParameter() = default;

StructAlreadyDefined::StructAlreadyDefined(const std::string& struct_name)
    : CompileError("Compile time error: struct with name (" + struct_name +
                   ") was defined more than once") {}

StructAlreadyDefined::~StructAlreadyDefined() = default;

UndefinedStruct::UndefinedStruct(const std::string& struct_name)
    : CompileError("Compile time error: struct with name (" + struct_name +
                   ") is undefined") {}

UndefinedStruct::~UndefinedStruct() = default;

TooManyParameters::TooManyParameters(const std::string& func_name,
                                     const int& expected_num_of_params)
    : CompileError("Compile time error: function (" + func_name + ") expects " +
                   std::to_string(expected_num_of_params) +
                   " number of parameters, but more were given") {};

TooManyParameters::~TooManyParameters() = default;

TooFewParameters::TooFewParameters(const std::string& func_name,
                                   const int& expected_num_of_params,
                                   const int& given_num_of_params)
    : CompileError("Compile time error: function with name (" + func_name + ") expected " +
                   std::to_string(expected_num_of_params) + " parameters, but " +
                   ((given_num_of_params == 0)
                        ? "none were given"
                        : ((given_num_of_params == 1)
                               ? "only 1 was given"
                               : "only " + std::to_string(given_num_of_params) +
                                     " were given"))) {};

TooFewParameters::~TooFewParameters() = default;

InvalidTypes::InvalidTypes() : CompileError("Compile time error: invalid types"){};
InvalidTypes::~InvalidTypes() = default;

InvalidFunctionCall::InvalidFunctionCall() : CompileError("Compile time error: invalid function call") {}
InvalidFunctionCall::~InvalidFunctionCall() = default;

CrementedRvalue::CrementedRvalue() : CompileError("Compile time error: can't ++ or -- an rvalue object") {}
CrementedRvalue::~CrementedRvalue() = default;

InvalidCrementOperand::InvalidCrementOperand() : CompileError("Compile time error: can't __crement anything, but \"double\" and \"int\"") {}
InvalidCrementOperand::~InvalidCrementOperand() = default;

InvalidUnaryOperand::InvalidUnaryOperand() : CompileError("Compile time error: can't do unary operation with custom types") {}
InvalidUnaryOperand::~InvalidUnaryOperand() = default;

InvalidBinaryNotOperand::InvalidBinaryNotOperand() : CompileError("Compile time error: can't do \"~\" to anything, but \"int\" and \"bool\"") {}
InvalidBinaryNotOperand::~InvalidBinaryNotOperand() = default;

AssignmentOfNonBasicTypes::AssignmentOfNonBasicTypes() : CompileError("Compile time error: can't do assignment with non basic types") {}
AssignmentOfNonBasicTypes::~AssignmentOfNonBasicTypes() = default;

AssignmentToRvalueError::AssignmentToRvalueError()
    : CompileError("Compile time error: can't assign value to an rvalue") {}

AssignmentToRvalueError::~AssignmentToRvalueError() = default;

InputNonLvalueObjectError::InputNonLvalueObjectError() : CompileError("Compile time error: can't input a non-lvalue object") {}
InputNonLvalueObjectError::~InputNonLvalueObjectError() = default;

ExpectedIntegerLiteral::ExpectedIntegerLiteral() : CompileError("Compile time error: expected integer literal") {}
ExpectedIntegerLiteral::~ExpectedIntegerLiteral() = default;

InvalidArraySize::InvalidArraySize() : CompileError("Compile time error: the size of the array is invalid") {}
InvalidArraySize::~InvalidArraySize() = default;

InvalidComparison::InvalidComparison() : CompileError("Compile time error: can't compare non-basic types") {}
InvalidComparison::~InvalidComparison() = default;
