#include "SyntaxError.h"

// Syntax Error

SyntaxError::SyntaxError(const std::string& ErrorText)
    : ErrorText_(ErrorText){};

SyntaxError::~SyntaxError() = default;

void SyntaxError::what() { std::cerr << ErrorText_; }

// Undefined Main Function

UndefinedMainFunctionError::UndefinedMainFunctionError()
    : SyntaxError(
          "Error: the main function isn't defined, or is defined improperly"){};

UndefinedMainFunctionError::~UndefinedMainFunctionError() = default;

// Invalid Filename

InvalidFilename::InvalidFilename()
    : SyntaxError("Error: the filename is written improperly") {}

InvalidFilename::~InvalidFilename() = default;

// Undefined Function Parameters

UndefinedFunctionParameters::UndefinedFunctionParameters()
    : SyntaxError(
          "Error: parameters of the function are undefined, or defined "
          "improperly") {}

UndefinedFunctionParameters::~UndefinedFunctionParameters() = default;

// Invalid Function Call

InvalidArgumentList::InvalidArgumentList()
    : SyntaxError("Error: argument list of the function is used improperly") {}

InvalidArgumentList::~InvalidArgumentList() = default;

// Invalid Struct Definition

InvalidStructDefinition::InvalidStructDefinition()
    : SyntaxError("Error: invalid struct definition") {}

InvalidStructDefinition::~InvalidStructDefinition() = default;

// Invalid Function Definition

InvalidFunctionDefinition::InvalidFunctionDefinition()
    : SyntaxError("Error: invalid function definition") {}

InvalidFunctionDefinition::~InvalidFunctionDefinition() = default;

// Invalid Struct Member

InvalidStructMember::InvalidStructMember()
    : SyntaxError("Error : struct member is used improperly") {}

InvalidStructMember::~InvalidStructMember() = default;

// Invalid Symbol

InvalidSymbol::InvalidSymbol() : SyntaxError("Error: invalid symbol is used") {}

InvalidSymbol::~InvalidSymbol() = default;

// Invalid Name

InvalidName::InvalidName() : SyntaxError("Error: invalid name") {}

InvalidName::~InvalidName() = default;

// Invalid Array Indexation

InvalidArrayIndexation::InvalidArrayIndexation()
    : SyntaxError("Error: array indexation is written improperly") {}

InvalidArrayIndexation::~InvalidArrayIndexation() = default;

// Missing a semicolumn

MissingSemicolumn::MissingSemicolumn()
    : SyntaxError("Error: missing a \";\""){};

MissingSemicolumn::~MissingSemicolumn() = default;

// Invalid Variable Creation

InvalidVariableCreation::InvalidVariableCreation()
    : SyntaxError("Error: creation of the variable is written improperly") {}

InvalidVariableCreation::~InvalidVariableCreation() = default;

// Invalid Unsigned Number

InvalidUnsignedNumber::InvalidUnsignedNumber()
    : SyntaxError("Error: invalid unsigned number") {}

InvalidUnsignedNumber::~InvalidUnsignedNumber() = default;

// Invalid Arithmetic Term

InvalidArithmeticTerm::InvalidArithmeticTerm()
    : SyntaxError("Error: invalid arithmetic term") {}

InvalidArithmeticTerm::~InvalidArithmeticTerm() = default;

InvalidArithmeticLiteral::InvalidArithmeticLiteral()
    : SyntaxError("Error: invalid arithmetic literal") {}

InvalidArithmeticLiteral::~InvalidArithmeticLiteral() = default;

ExpectedOpenParenthesis::ExpectedOpenParenthesis()
    : SyntaxError("Error: expected a \"(\"") {}

ExpectedOpenParenthesis::~ExpectedOpenParenthesis() = default;

ExpectedCloseParenthesis::ExpectedCloseParenthesis()
    : SyntaxError("Error: expected a \")\""){};

ExpectedCloseParenthesis::~ExpectedCloseParenthesis() = default;

ExpectedDo::ExpectedDo() : SyntaxError("Error: expected \"do\""){};

ExpectedDo::~ExpectedDo() = default;

ExpectedFigureOpen::ExpectedFigureOpen()
    : SyntaxError("Error: expected \"{\""){};

ExpectedFigureOpen::~ExpectedFigureOpen() = default;

ExpectedFigureClose::ExpectedFigureClose()
    : SyntaxError("Error: expected \"}\""){};

ExpectedFigureClose::~ExpectedFigureClose() = default;

ExpectedMatch::ExpectedMatch() : SyntaxError("Error: expected \">=\""){};

ExpectedMatch::~ExpectedMatch() = default;

ExpectedSquareOpen::ExpectedSquareOpen()
    : SyntaxError("Error: expected \"[\""){};

ExpectedSquareOpen::~ExpectedSquareOpen() = default;

ExpectedSquareClose::ExpectedSquareClose()
    : SyntaxError("Error: expected \"]\""){};

ExpectedSquareClose::~ExpectedSquareClose() = default;

ExpectedComma::ExpectedComma() : SyntaxError("Error: expected \",\""){};

ExpectedComma::~ExpectedComma() = default;

ExpectedEqual::ExpectedEqual() : SyntaxError("Error: expected \"=\""){};

ExpectedEqual::~ExpectedEqual() = default;

ImpossibleLexem::ImpossibleLexem()
    : SyntaxError("Error: impossible to allocate a lexem"){};

ImpossibleLexem::~ImpossibleLexem() = default;

InvalidStringLiteral::InvalidStringLiteral()
    : SyntaxError("Error: invalid string literal"){};

InvalidStringLiteral::~InvalidStringLiteral() = default;

// InvalidInitialization::InvalidInitialization()
//     : SyntaxError("Error: invalid initialization"){};

// InvalidInitialization::~InvalidInitialization() = default;