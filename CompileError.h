#pragma once
#include <iostream>
#include <string>

class CompileError {
   public:
    void what();
    CompileError(const std::string& ErrorText);

    virtual ~CompileError();

   private:
    std::string ErrorText_;
};

// ----------------------- SYNTAX ERRORS -----------------------

class UndefinedMainFunctionError : public CompileError {
   public:
    UndefinedMainFunctionError();

    virtual ~UndefinedMainFunctionError();
};

class InvalidFilename : public CompileError {
   public:
    InvalidFilename();

    virtual ~InvalidFilename();
};

class UndefinedFunctionParameters : public CompileError {
   public:
    UndefinedFunctionParameters();

    virtual ~UndefinedFunctionParameters();
};

class InvalidFunctionDefinition : public CompileError {
   public:
    InvalidFunctionDefinition();

    virtual ~InvalidFunctionDefinition();
};

class InvalidArgumentList : public CompileError {
   public:
    InvalidArgumentList();

    virtual ~InvalidArgumentList();
};

class InvalidStructDefinition : public CompileError {
   public:
    InvalidStructDefinition();

    virtual ~InvalidStructDefinition();
};

class InvalidStructMember : public CompileError {
   public:
    InvalidStructMember();

    virtual ~InvalidStructMember();
};

class InvalidSymbol : public CompileError {
   public:
    InvalidSymbol();

    virtual ~InvalidSymbol();
};

class InvalidName : public CompileError {
   public:
    InvalidName();

    virtual ~InvalidName();
};

class InvalidArrayIndexation : public CompileError {
   public:
    InvalidArrayIndexation();

    virtual ~InvalidArrayIndexation();
};

class MissingSemicolumn : public CompileError {
   public:
    MissingSemicolumn();

    virtual ~MissingSemicolumn();
};

class InvalidVariableCreation : public CompileError {
   public:
    InvalidVariableCreation();

    virtual ~InvalidVariableCreation();
};

class InvalidUnsignedNumber : public CompileError {
   public:
    InvalidUnsignedNumber();

    virtual ~InvalidUnsignedNumber();
};

class InvalidArithmeticTerm : public CompileError {
   public:
    InvalidArithmeticTerm();

    virtual ~InvalidArithmeticTerm();
};

class InvalidArithmeticLiteral : public CompileError {
   public:
    InvalidArithmeticLiteral();

    virtual ~InvalidArithmeticLiteral();
};

class ExpectedOpenParenthesis : public CompileError {
   public:
    ExpectedOpenParenthesis();
    virtual ~ExpectedOpenParenthesis();
};

class ExpectedCloseParenthesis : public CompileError {
   public:
    ExpectedCloseParenthesis();
    virtual ~ExpectedCloseParenthesis();
};

class ExpectedDo : public CompileError {
   public:
    ExpectedDo();
    virtual ~ExpectedDo();
};

class ExpectedFigureOpen : public CompileError {
   public:
    ExpectedFigureOpen();
    virtual ~ExpectedFigureOpen();
};

class ExpectedFigureClose : public CompileError {
   public:
    ExpectedFigureClose();
    virtual ~ExpectedFigureClose();
};

class ExpectedMatch : public CompileError {
   public:
    ExpectedMatch();
    virtual ~ExpectedMatch();
};

class ExpectedSquareOpen : public CompileError {
   public:
    ExpectedSquareOpen();
    virtual ~ExpectedSquareOpen();
};

class ExpectedSquareClose : public CompileError {
   public:
    ExpectedSquareClose();
    virtual ~ExpectedSquareClose();
};

class ExpectedComma : public CompileError {
   public:
    ExpectedComma();
    virtual ~ExpectedComma();
};

class ExpectedEqual : public CompileError {
   public:
    ExpectedEqual();
    virtual ~ExpectedEqual();
};

class ImpossibleLexem : public CompileError {
   public:
    ImpossibleLexem();
    virtual ~ImpossibleLexem();
};

class InvalidStringLiteral : public CompileError {
   public:
    InvalidStringLiteral();
    virtual ~InvalidStringLiteral();
};

// class InvalidInitialization: public CompileError{
//     public:
//     InvalidInitialization();
//     virtual ~InvalidInitialization();
// };

// ----------------------- SEMANTIC ERRORS -----------------------

class IdentifierAlreadyDefined : public CompileError {
public:
    IdentifierAlreadyDefined(const std::string& VariableName);
    
    virtual ~IdentifierAlreadyDefined();
};

class UndefinedVariable : public CompileError {
public:
    UndefinedVariable(const std::string& VariableName);

    virtual ~UndefinedVariable();
};

class FunctionAlreadyDefined : public CompileError {
public:
    
    FunctionAlreadyDefined(const std::string& FunctionName);

    virtual ~FunctionAlreadyDefined();
};

class UndefinedFunction : public CompileError {
public:

    UndefinedFunction(const std::string& FunctionName);

    virtual ~UndefinedFunction();
};

class InvalidParameter : public CompileError {
public:
    
    InvalidParameter(const std::string& FunctionName, const std::string& ExpectedType, const std::string& GivenType);

    virtual ~InvalidParameter();
};

class StructAlreadyDefined : public CompileError {
public:
    StructAlreadyDefined(const std::string& struct_name);

    virtual ~StructAlreadyDefined();
};

class UndefinedStruct : public CompileError {
public:
    UndefinedStruct(const std::string& struct_name);

    virtual ~UndefinedStruct();
};

class TooManyParameters : public CompileError {
public:
    TooManyParameters(const std::string& func_name, const int& expected_num_of_paramss);

    virtual ~TooManyParameters();
};

class TooFewParameters : public CompileError {
public:
    TooFewParameters(const std::string& func_name, const int& expected_num_of_params, const int& given_num_of_params);

    virtual ~TooFewParameters();
};
