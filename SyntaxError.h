#pragma once
#include <iostream>
#include <string>

class SyntaxError {
   public:
    void what();
    SyntaxError(const std::string& ErrorText);

    virtual ~SyntaxError();

   private:
    std::string ErrorText_;
};

class UndefinedMainFunctionError : public SyntaxError {
   public:
    UndefinedMainFunctionError();

    virtual ~UndefinedMainFunctionError();
};

class InvalidFilename : public SyntaxError {
   public:
    InvalidFilename();

    virtual ~InvalidFilename();
};

class UndefinedFunctionParameters : public SyntaxError {
   public:
    UndefinedFunctionParameters();

    virtual ~UndefinedFunctionParameters();
};

class InvalidFunctionDefinition : public SyntaxError {
   public:
    InvalidFunctionDefinition();

    virtual ~InvalidFunctionDefinition();
};

class InvalidArgumentList : public SyntaxError {
   public:
    InvalidArgumentList();

    virtual ~InvalidArgumentList();
};

class InvalidStructDefinition : public SyntaxError {
   public:
    InvalidStructDefinition();

    virtual ~InvalidStructDefinition();
};

class InvalidStructMember : public SyntaxError {
   public:
    InvalidStructMember();

    virtual ~InvalidStructMember();
};

class InvalidSymbol : public SyntaxError {
   public:
    InvalidSymbol();

    virtual ~InvalidSymbol();
};

class InvalidName : public SyntaxError {
   public:
    InvalidName();

    virtual ~InvalidName();
};

class InvalidArrayIndexation : public SyntaxError {
   public:
    InvalidArrayIndexation();

    virtual ~InvalidArrayIndexation();
};

class MissingSemicolumn : public SyntaxError {
   public:
    MissingSemicolumn();

    virtual ~MissingSemicolumn();
};

class InvalidVariableCreation : public SyntaxError {
   public:
    InvalidVariableCreation();

    virtual ~InvalidVariableCreation();
};

class InvalidUnsignedNumber : public SyntaxError {
   public:
    InvalidUnsignedNumber();

    virtual ~InvalidUnsignedNumber();
};

class InvalidArithmeticTerm : public SyntaxError {
   public:
    InvalidArithmeticTerm();

    virtual ~InvalidArithmeticTerm();
};

class InvalidArithmeticLiteral : public SyntaxError {
   public:
    InvalidArithmeticLiteral();

    virtual ~InvalidArithmeticLiteral();
};

class ExpectedOpenParenthesis : public SyntaxError {
   public:
    ExpectedOpenParenthesis();
    virtual ~ExpectedOpenParenthesis();
};

class ExpectedCloseParenthesis : public SyntaxError {
   public:
    ExpectedCloseParenthesis();
    virtual ~ExpectedCloseParenthesis();
};

class ExpectedDo : public SyntaxError {
   public:
    ExpectedDo();
    virtual ~ExpectedDo();
};

class ExpectedFigureOpen : public SyntaxError {
   public:
    ExpectedFigureOpen();
    virtual ~ExpectedFigureOpen();
};

class ExpectedFigureClose : public SyntaxError {
   public:
    ExpectedFigureClose();
    virtual ~ExpectedFigureClose();
};

class ExpectedMatch : public SyntaxError {
   public:
    ExpectedMatch();
    virtual ~ExpectedMatch();
};

class ExpectedSquareOpen : public SyntaxError {
   public:
    ExpectedSquareOpen();
    virtual ~ExpectedSquareOpen();
};

class ExpectedSquareClose : public SyntaxError {
   public:
    ExpectedSquareClose();
    virtual ~ExpectedSquareClose();
};

class ExpectedComma : public SyntaxError {
   public:
    ExpectedComma();
    virtual ~ExpectedComma();
};

class ExpectedEqual : public SyntaxError {
   public:
    ExpectedEqual();
    virtual ~ExpectedEqual();
};

class ImpossibleLexem : public SyntaxError {
   public:
    ImpossibleLexem();
    virtual ~ImpossibleLexem();
};
