#include "SintaxError.hpp"

// Sintax Error

SintaxError::SintaxError(const std::string& ErrorText) : ErrorText_(ErrorText) {};

SintaxError::~SintaxError() = default;

void SintaxError::what()
{
    std::cerr << ErrorText_;
}

// Undefined Main Function

UndefinedMainFunctionError::UndefinedMainFunctionError() 
    : SintaxError("Error: the main function isn't defined, or is defined improperly") {};

UndefinedMainFunctionError::~UndefinedMainFunctionError() = default;

// Invalid Filename

InvalidFilename::InvalidFilename() 
    : SintaxError("Error: the filename is written improperly") {}

InvalidFilename::~InvalidFilename() = default;

// Undefined Function Parameters

UndefinedFunctionParameters::UndefinedFunctionParameters()
    : SintaxError("Error: parameters of the function are undefined, or defined improperly") {}

UndefinedFunctionParameters::~UndefinedFunctionParameters() = default;

// Invalid Function Call

InvalidArgumentList::InvalidArgumentList() 
    : SintaxError("Error: argument list of the function is used improperly") {}

InvalidArgumentList::~InvalidArgumentList() = default;

// Invalid Struct Definition

InvalidStructDefinition::InvalidStructDefinition()
    : SintaxError("Error: invalid struct definition") {}

InvalidStructDefinition::~InvalidStructDefinition() = default;

// Invalid Function Definition

InvalidFunctionDefinition::InvalidFunctionDefinition() 
    : SintaxError("Error: invalid function definition") {}

InvalidFunctionDefinition::~InvalidFunctionDefinition() = default;

// Invalid Struct Member

InvalidStructMember::InvalidStructMember()
    : SintaxError("Error : struct member is used improperly") {}

InvalidStructMember::~InvalidStructMember() = default;

// Invalid Symbol

InvalidSymbol::InvalidSymbol()
    : SintaxError("Error: invalid symbol is used") {}

InvalidSymbol::~InvalidSymbol() = default;

// Invalid Name

InvalidName::InvalidName()
    : SintaxError("Error: invalid name") {}

InvalidName::~InvalidName() = default;

// Invalid Array Indexation

InvalidArrayIndexation::InvalidArrayIndexation()
    : SintaxError("Error: array indexation is written improperly") {}

InvalidArrayIndexation::~InvalidArrayIndexation() = default;

// Missing a semicolumn

MissingSemicolumn::MissingSemicolumn()
    : SintaxError("Error: missing a semicolumn") {};

MissingSemicolumn::~MissingSemicolumn() = default;

// Invalid Variable Creation

InvalidVariableCreation::InvalidVariableCreation()
    : SintaxError("Error: creation of the variable is written improperly") {}

InvalidVariableCreation::~InvalidVariableCreation() = default;

// Invalid Unsigned Number

InvalidUnsignedNumber::InvalidUnsignedNumber() 
    : SintaxError("Error: invalid unsigned number") {}

InvalidUnsignedNumber::~InvalidUnsignedNumber() = default;

// Invalid Arithmetic Term

InvalidArithmeticTerm::InvalidArithmeticTerm()
    : SintaxError("Error: invalid arithmetic term") {}

InvalidArithmeticTerm::~InvalidArithmeticTerm() = default;

InvalidArithmeticLiteral::InvalidArithmeticLiteral()
    : SintaxError("Error: invalid arithmetic literal") {}

InvalidArithmeticLiteral::~InvalidArithmeticLiteral() = default;
