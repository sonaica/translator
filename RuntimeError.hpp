#pragma once
#include <iostream>

class RuntimeError {
  public:
    void what();
    RuntimeError(const std::string& ErrorText);

    virtual ~RuntimeError();

   private:
    std::string ErrorText_;
};

class DivisionByZeroError : public RuntimeError {
  public:
    DivisionByZeroError();

    virtual ~DivisionByZeroError();
};

class ModFromDoubleError : public RuntimeError {
  public:
    ModFromDoubleError();

    virtual ~ModFromDoubleError();
};

class ZeroRaisedToOrbitError: public RuntimeError {
  public:
    ZeroRaisedToOrbitError();

    virtual ~ZeroRaisedToOrbitError();
};

class BitwiseOpsWithDoublesError: public RuntimeError {
  public:
    BitwiseOpsWithDoublesError();

    virtual ~BitwiseOpsWithDoublesError();
};

class InvalidOutputOperandError: public RuntimeError {
  public:
    InvalidOutputOperandError();

    virtual ~InvalidOutputOperandError();
};

class CrementOnBoolError: public RuntimeError {
  public:
    CrementOnBoolError();

    virtual ~CrementOnBoolError();
};