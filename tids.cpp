#pragma once
#include "tids.h"

#include "CompileError.cpp"

// struct Value

Value::Value() : type_(""), name_("") {}

Value::Value(const std::string& type, const std::string& name)
    : type_(type), name_(name) {}

std::string Value::name() const { return name_; }

std::string Value::type() const { return type_; }

bool Value::operator==(const Value& other) const {
    return other.type() == type() && other.name() == name();
}

bool duplicates(const Value& first, const Value& second) {
    return first.name() == second.name();
}

// class IdentifierTIDS

IdentifierTIDS::element::~element() {

}

IdentifierTIDS::element::element(element *parent)
    : parent_(parent), variables_({}) {}

void IdentifierTIDS::element::push_id(const Value& Value) {
    if (name_set.find(Value.name_).first)
        throw IdentifierAlreadyDefined(Value.name());
    variables_.push_back(Value);
    name_set.insert(Value.name_, Value.type_);
    adr_name_set.insert(Value.name_, new cool_byte);
}

std::string IdentifierTIDS::element::check_id(const std::string& VariableName) {
    std::pair<bool, std::string&> find_result = name_set.find(VariableName);
    if (!find_result.first) {
        if (parent_ == nullptr) throw UndefinedVariable(VariableName);
        return parent_->check_id(VariableName);
    }
    return find_result.second;
}

cool_byte*& IdentifierTIDS::element::find_id(const std::string &VariableName)
{
    std::pair<bool, cool_byte*&> find_result = adr_name_set.find(VariableName);
    if (!find_result.first) {
        if (parent_ == nullptr) throw UndefinedVariable(VariableName);
        return parent_->find_id(VariableName);
    }
    return find_result.second;
}

IdentifierTIDS::IdentifierTIDS()
    : cur_tid_(new IdentifierTIDS::element(nullptr)) {}

IdentifierTIDS::element*& IdentifierTIDS::cur_tid() { return cur_tid_; }

void IdentifierTIDS::create_TID() {
    cur_tid_ = new element(cur_tid_);
}

void IdentifierTIDS::del_TID() {
    IdentifierTIDS::element* q = cur_tid_;
    cur_tid_ = cur_tid_->parent_;
    delete q;
}

bool IdentifierTIDS::is_empty() const
{
    return cur_tid_->parent_ == nullptr;
}

// struct Function

bool duplicates(Function& F1, Function F2) {
    if (F1.name() != F2.name()) return false;
    if (F1.argument_list() != F2.argument_list()) return false;
    return true;
}

Function::Function(std::string name, size_t poliz_pos) : name_(name), poliz_pos_(poliz_pos) {}

const std::string& Function::return_type() const { return return_type_; }

const std::string& Function::name() const { return name_; }

std::vector<Value>& Function::argument_list() { return argument_list_; }

size_t Function::poliz_pos() const { return poliz_pos_; }

void Function::set_poliz_pos(const size_t& new_pos) {
    poliz_pos_ = new_pos;
}

// class FunctionTIDS

FunctionTIDS::FunctionTIDS() {}

void FunctionTIDS::push_func_id(const std::string& func_name) {
    std::pair<bool, Function&> find_result = name_set.find(func_name);
    if (find_result.first) throw FunctionAlreadyDefined(func_name);
    name_set.insert(func_name, Function(func_name));
}

std::string FunctionTIDS::check_func_id(const std::string& func_name) {
    std::pair<bool, Function&> find_result = name_set.find(func_name);
    if (!find_result.first) throw UndefinedFunction(func_name);
    return find_result.second.return_type();
}

void FunctionTIDS::push_func_return_type(const std::string& func_name,
                                         const std::string& return_type) {
    std::pair<bool, Function&> find_result = name_set.find(func_name);
    if (!find_result.first) throw UndefinedFunction(func_name);
    find_result.second.set_return_type(return_type);
}

void FunctionTIDS::push_func_poliz_pos(const std::string& func_name,
                                       const std::size_t& pos) {
    std::pair<bool, Function&> find_result = name_set.find(func_name);
    if (!find_result.first) throw UndefinedFunction(func_name);
    find_result.second.set_poliz_pos(pos);
}

void FunctionTIDS::check_param_count(const std::string& func_name,
                                     const int& have_params) {
    std::pair<bool, Function&> find_result = name_set.find(func_name);
    if (!find_result.first) throw UndefinedFunction(func_name);
    if (have_params < (int)find_result.second.argument_list().size())
        throw TooFewParameters(
            func_name, find_result.second.argument_list().size(), have_params);
}

void FunctionTIDS::check_exist_id(const std::string& name) {
    std::pair<bool, Function&> find_result = name_set.find(name);
    if (find_result.first) throw FunctionAlreadyDefined(name);
    return;
}

Function &FunctionTIDS::getFunction(const std::string &name)
{
    return name_set.find(name).second;
}

void FunctionTIDS::check_func_par(const std::string& func_name, int par_num,
                                  const std::string& type) {
    std::pair<bool, Function&> find_result = name_set.find(func_name);
    if (!find_result.first) throw UndefinedFunction(func_name);
    if (par_num >= (int)find_result.second.argument_list().size())
        throw TooManyParameters(func_name,
                                (int)find_result.second.argument_list().size());
    if (find_result.second.argument_list()[par_num].type() != type)
        throw InvalidParameter(
            func_name, find_result.second.argument_list()[par_num].type(),
            type);
}

void FunctionTIDS::push_func_par(const std::string& func_name,
                                 const Value& variable) {
    std::pair<bool, Function&> find_res = name_set.find(func_name);
    if (!find_res.first) throw UndefinedFunction(func_name);
    Function& fun = find_res.second;
    std::pair<bool, int&> par_num = fun.name_set.find(variable.name());
    if (par_num.first) throw IdentifierAlreadyDefined(variable.name());
    fun.name_set.insert(variable.name(), (int)fun.argument_list().size());
    fun.argument_list().push_back(variable);
}

// class StructTIDS

StructTIDS::StructTIDS() {}

void StructTIDS::push_id(const std::string& struct_name,
                         const Value& variable) {
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    find_res.second.second.cur_tid()->push_id(variable);
}

bool StructTIDS::check_struct_existance(const std::string &struct_name)
{
    return name_set.find(struct_name).first;
}

std::string StructTIDS::check_id(const std::string& struct_name,
                                 const std::string& name) {
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    return find_res.second.second.cur_tid()->check_id(name);
}

void StructTIDS::push_struct_id(const std::string& struct_name) {
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (find_res.first) throw StructAlreadyDefined(struct_name);
    name_set.insert(struct_name);
}

void StructTIDS::check_struct_id(const std::string& struct_name) {
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
}

void StructTIDS::push_func_id(const std::string& struct_name,
                              const std::string& func_name) {
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    find_res.second.first.push_func_id(func_name);
}

std::string StructTIDS::check_func_id(const std::string& struct_name,
                                      const std::string& func_name) {
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    return find_res.second.first.check_func_id(func_name);
}

void StructTIDS::push_func_return_type(const std::string& struct_name,
                                       const std::string& func_name,
                                       const std::string& return_type) {
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    find_res.second.first.push_func_return_type(func_name, return_type);
}

void StructTIDS::push_func_poliz_pos(const std::string& struct_name,
                                     const std::string& func_name,
                                     const std::size_t& pos) {
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res = 
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    find_res.second.first.push_func_poliz_pos(func_name, pos);
}

void StructTIDS::check_func_par(const std::string& struct_name,
                                const std::string& func_name, int par_num,
                                const std::string& type) {
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    find_res.second.first.check_func_par(func_name, par_num, type);
}

void StructTIDS::push_func_par(const std::string& struct_name,
                               const std::string& func_name,
                               const Value& variable) {
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    find_res.second.first.push_func_par(func_name, variable);
}

void StructTIDS::check_param_count(const std::string& struct_name,
                                   const std::string& func_name,
                                   const int& have_params) {
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    find_res.second.first.check_param_count(func_name, have_params);
}

void Value::set_name(const std::string& name) { name_ = name; }

void Value::set_type(const std::string& type) { type_ = type; }

std::string Value::get_type() { return type_; }

void Function::set_name(const std::string& name) { name_ = name; }

void Function::set_return_type(const std::string& type) { return_type_ = type; }
