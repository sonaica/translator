#pragma once
#include "CompileError.h"

CompileError::CompileError(const std::string& ErrorText)
    : ErrorText_(ErrorText){};

CompileError::~CompileError() = default;

void CompileError::what() { std::cerr << ErrorText_; }

// ----------------------- SYNTAX ERRORS -----------------------

// Undefined Main Function

UndefinedMainFunctionError::UndefinedMainFunctionError()
    : CompileError(
          "Error: the main function isn't defined, or is defined improperly"){};

UndefinedMainFunctionError::~UndefinedMainFunctionError() = default;

// Invalid Filename

InvalidFilename::InvalidFilename()
    : CompileError("Error: the filename is written improperly") {}

InvalidFilename::~InvalidFilename() = default;

// Undefined Function Parameters

UndefinedFunctionParameters::UndefinedFunctionParameters()
    : CompileError(
          "Error: parameters of the function are undefined, or defined "
          "improperly") {}

UndefinedFunctionParameters::~UndefinedFunctionParameters() = default;

// Invalid Function Call

InvalidArgumentList::InvalidArgumentList()
    : CompileError("Error: argument list of the function is used improperly") {}

InvalidArgumentList::~InvalidArgumentList() = default;

// Invalid Struct Definition

InvalidStructDefinition::InvalidStructDefinition()
    : CompileError("Error: invalid struct definition") {}

InvalidStructDefinition::~InvalidStructDefinition() = default;

// Invalid Function Definition

InvalidFunctionDefinition::InvalidFunctionDefinition()
    : CompileError("Error: invalid function definition") {}

InvalidFunctionDefinition::~InvalidFunctionDefinition() = default;

// Invalid Struct Member

InvalidStructMember::InvalidStructMember()
    : CompileError("Error : struct member is used improperly") {}

InvalidStructMember::~InvalidStructMember() = default;

// Invalid Symbol

InvalidSymbol::InvalidSymbol()
    : CompileError("Error: invalid symbol is used") {}

InvalidSymbol::~InvalidSymbol() = default;

// Invalid Name

InvalidName::InvalidName() : CompileError("Error: invalid name") {}

InvalidName::~InvalidName() = default;

// Invalid Array Indexation

InvalidArrayIndexation::InvalidArrayIndexation()
    : CompileError("Error: array indexation is written improperly") {}

InvalidArrayIndexation::~InvalidArrayIndexation() = default;

// Missing a semicolumn

MissingSemicolumn::MissingSemicolumn()
    : CompileError("Error: missing a \";\""){};

MissingSemicolumn::~MissingSemicolumn() = default;

// Invalid Variable Creation

InvalidVariableCreation::InvalidVariableCreation()
    : CompileError("Error: creation of the variable is written improperly") {}

InvalidVariableCreation::~InvalidVariableCreation() = default;

// Invalid Unsigned Number

InvalidUnsignedNumber::InvalidUnsignedNumber()
    : CompileError("Error: invalid unsigned number") {}

InvalidUnsignedNumber::~InvalidUnsignedNumber() = default;

// Invalid Arithmetic Term

InvalidArithmeticTerm::InvalidArithmeticTerm()
    : CompileError("Error: invalid arithmetic term") {}

InvalidArithmeticTerm::~InvalidArithmeticTerm() = default;

InvalidArithmeticLiteral::InvalidArithmeticLiteral()
    : CompileError("Error: invalid arithmetic literal") {}

InvalidArithmeticLiteral::~InvalidArithmeticLiteral() = default;

ExpectedOpenParenthesis::ExpectedOpenParenthesis()
    : CompileError("Error: expected a \"(\"") {}

ExpectedOpenParenthesis::~ExpectedOpenParenthesis() = default;

ExpectedCloseParenthesis::ExpectedCloseParenthesis()
    : CompileError("Error: expected a \")\""){};

ExpectedCloseParenthesis::~ExpectedCloseParenthesis() = default;

ExpectedDo::ExpectedDo() : CompileError("Error: expected \"do\""){};

ExpectedDo::~ExpectedDo() = default;

ExpectedFigureOpen::ExpectedFigureOpen()
    : CompileError("Error: expected \"{\""){};

ExpectedFigureOpen::~ExpectedFigureOpen() = default;

ExpectedFigureClose::ExpectedFigureClose()
    : CompileError("Error: expected \"}\""){};

ExpectedFigureClose::~ExpectedFigureClose() = default;

ExpectedMatch::ExpectedMatch() : CompileError("Error: expected \">=\""){};

ExpectedMatch::~ExpectedMatch() = default;

ExpectedSquareOpen::ExpectedSquareOpen()
    : CompileError("Error: expected \"[\""){};

ExpectedSquareOpen::~ExpectedSquareOpen() = default;

ExpectedSquareClose::ExpectedSquareClose()
    : CompileError("Error: expected \"]\""){};

ExpectedSquareClose::~ExpectedSquareClose() = default;

ExpectedComma::ExpectedComma() : CompileError("Error: expected \",\""){};

ExpectedComma::~ExpectedComma() = default;

ExpectedEqual::ExpectedEqual() : CompileError("Error: expected \"=\""){};

ExpectedEqual::~ExpectedEqual() = default;

ImpossibleLexem::ImpossibleLexem()
    : CompileError("Error: impossible to allocate a lexem"){};

ImpossibleLexem::~ImpossibleLexem() = default;

InvalidStringLiteral::InvalidStringLiteral()
    : CompileError("Error: invalid string literal"){};

InvalidStringLiteral::~InvalidStringLiteral() = default;

// InvalidInitialization::InvalidInitialization()
//     : CompileError("Error: invalid initialization"){};

// InvalidInitialization::~InvalidInitialization() = default;

// ----------------------- SEMANTIC ERRORS -----------------------

IdentifierAlreadyDefined::IdentifierAlreadyDefined(
    const std::string& VariableName)
    : CompileError("Error: variable with name (" + VariableName +
                   ") was defined more than once") {}

IdentifierAlreadyDefined::~IdentifierAlreadyDefined() = default;

UndefinedVariable::UndefinedVariable(const std::string& VariableName)
    : CompileError("Error: variable with name (" + VariableName +
                   ") is undefined") {}

UndefinedVariable::~UndefinedVariable() = default;

FunctionAlreadyDefined::FunctionAlreadyDefined(const std::string& FunctionName)
    : CompileError("Error: function with name (" + FunctionName +
                   ") was defined more than once") {}

FunctionAlreadyDefined::~FunctionAlreadyDefined() {}

UndefinedFunction::UndefinedFunction(const std::string& FunctionName)
    : CompileError("Error: function with name (" + FunctionName +
                   ") is undefined") {}

UndefinedFunction::~UndefinedFunction() = default;

InvalidParameter::InvalidParameter(const std::string& FunctionName,
                                   const std::string& ExpectedType,
                                   const std::string& GivenType)
    : CompileError("Error: invalid parameter entered in function (" +
                   FunctionName + "). Expected (" + ExpectedType +
                   "), given (" + GivenType + ")") {}

InvalidParameter::~InvalidParameter() = default;

StructAlreadyDefined::StructAlreadyDefined(const std::string& struct_name)
    : CompileError("Error: struct with name (" + struct_name +
                   ") was defined more than once") {}

StructAlreadyDefined::~StructAlreadyDefined() = default;

UndefinedStruct::UndefinedStruct(const std::string& struct_name)
    : CompileError("Error: struct with name (" + struct_name +
                   ") is undefined") {}

UndefinedStruct::~UndefinedStruct() = default;

TooManyParameters::TooManyParameters(const std::string& func_name,
                                     const int& expected_num_of_params)
    : CompileError("Error: function (" + func_name + ") expects " +
                   std::to_string(expected_num_of_params) +
                   " number of parameters, but more were given") {};

TooManyParameters::~TooManyParameters() = default;

TooFewParameters::TooFewParameters(const std::string& func_name,
                                   const int& expected_num_of_params,
                                   const int& given_num_of_params)
    : CompileError("Error: function with name (" + func_name + ") expected " +
                   std::to_string(expected_num_of_params) + " parameters, but " +
                   ((given_num_of_params == 0)
                        ? "none were given"
                        : ((given_num_of_params == 1)
                               ? "only 1 was given"
                               : "only " + std::to_string(given_num_of_params) +
                                     " were given"))) {};

TooFewParameters::~TooFewParameters() = default;

InvalidTypes::InvalidTypes() : CompileError("Error: invalid types"){};
InvalidTypes::~InvalidTypes() = default;
