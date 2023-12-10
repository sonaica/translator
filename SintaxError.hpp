#include <string>
#include <iostream>

#include <string>

class SintaxError {
public:
    SintaxError(const std::string& ErrorText);
    
    virtual ~SintaxError();

    void what();

private:
    std::string ErrorText_;
};

class UndefinedMainFunctionError : public SintaxError {
public:
    UndefinedMainFunctionError();

    virtual ~UndefinedMainFunctionError();
};

class InvalidFilename : public SintaxError {
public:
    InvalidFilename();

    virtual ~InvalidFilename();
};

class UndefinedFunctionParameters : public SintaxError {
public:
    UndefinedFunctionParameters();
    
    virtual ~UndefinedFunctionParameters();
};

class InvalidFunctionDefinition : public SintaxError {
public:
    InvalidFunctionDefinition();

    virtual ~InvalidFunctionDefinition();
};

class InvalidArgumentList : public SintaxError {
public:
    InvalidArgumentList();

    virtual ~InvalidArgumentList();
};

class InvalidStructDefinition : public SintaxError {
public:
    InvalidStructDefinition();

    virtual ~InvalidStructDefinition();
};

class InvalidStructMember : public SintaxError {
public:
    InvalidStructMember();

    virtual ~InvalidStructMember();
};

class InvalidSymbol : public SintaxError {
public:
    InvalidSymbol();

    virtual ~InvalidSymbol();
};

class InvalidName : public SintaxError {
public:
    InvalidName();

    virtual ~InvalidName();
};

class InvalidArrayIndexation : public SintaxError {
public:
    InvalidArrayIndexation();

    virtual ~InvalidArrayIndexation();
};

class MissingSemicolumn : public SintaxError {
public:
    MissingSemicolumn();

    virtual ~MissingSemicolumn();
};

class InvalidVariableCreation : public SintaxError {
public:
    InvalidVariableCreation();

    virtual ~InvalidVariableCreation();
};

class InvalidUnsignedNumber : public SintaxError {
public:
    InvalidUnsignedNumber();

    virtual ~InvalidUnsignedNumber();
};

class InvalidArithmeticTerm : public SintaxError {
public:
    InvalidArithmeticTerm();

    virtual ~InvalidArithmeticTerm();
};

class InvalidArithmeticLiteral : public SintaxError {
public:
    InvalidArithmeticLiteral();

    virtual ~InvalidArithmeticLiteral();
};