#pragma once
#include "RuntimeError.hpp"

void RuntimeError::what() { std::cerr << ErrorText_; }

RuntimeError::RuntimeError(const std::string &ErrorText) : ErrorText_(ErrorText) {}

RuntimeError::~RuntimeError() = default;

DivisionByZeroError::DivisionByZeroError()
    : RuntimeError("Runtime error: division by zero") {}

DivisionByZeroError::~DivisionByZeroError() = default;

ModFromDoubleError::ModFromDoubleError()
    : RuntimeError("Runtime error: % (mod) from double can't be taken") {}

ModFromDoubleError::~ModFromDoubleError() = default;

ZeroRaisedToOrbitError::ZeroRaisedToOrbitError()
    : RuntimeError("Runtime error: can't raise 0 to the power <= 0") {}

ZeroRaisedToOrbitError::~ZeroRaisedToOrbitError() = default;

BitwiseOpsWithDoublesError::BitwiseOpsWithDoublesError()
    : RuntimeError("Runtime error: can't do bitwise operations with doubles") {}

BitwiseOpsWithDoublesError::~BitwiseOpsWithDoublesError() = default;

InvalidOutputOperandError::InvalidOutputOperandError()
    : RuntimeError("Runtime error: can't output this item") {}

InvalidOutputOperandError::~InvalidOutputOperandError() = default;

CrementOnBoolError::CrementOnBoolError()
    : RuntimeError("Runtime error : can't increment or decrement a boolean value") {}

CrementOnBoolError::~CrementOnBoolError() = default;