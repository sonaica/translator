#pragma once
#include "tids.h"
#include "common_functions.cpp"

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

size_t get_type_size(const std::string &str, StructTIDS& StrTIDS)
{
    if (str == "int" || str == "double")
        return sizeof(int64_t) / sizeof(cool_byte);
    if (str == "bool")
        return sizeof(cool_byte);

    return StrTIDS.get_struct_size(str);
}

bool duplicates(const Value &first, const Value &second)
{
    return first.name() == second.name();
}

// class IdentifierTIDS

IdentifierTIDS::element::~element() {

}

IdentifierTIDS::element::element(element *parent)
    : parent_(parent), variables_({}) {}

void IdentifierTIDS::element::push_id(const Value& Value, StructTIDS& StrTIDS, int cnt) {
    if (name_set.find(Value.name_).first)
        throw IdentifierAlreadyDefined(Value.name());
    variables_.push_back(Value);
    name_set.insert(Value.name_, Value.type_);
    name_move.insert(Value.name_, block_size);
    if (is_array(Value.type())) {
        size_t added_size = get_type_size(get_array_type(Value.type()), StrTIDS);
        adr_name_set.insert(Value.name_, new cool_byte[added_size * cnt]);
        block_size += added_size * cnt;
    } else {
        size_t added_size = get_type_size(Value.type(), StrTIDS);
        adr_name_set.insert(Value.name_, new cool_byte[added_size]);
        block_size += added_size;
    }
}

bool IdentifierTIDS::element::check_id_safe(const std::string &VariableName)
{
    std::pair<bool, std::string&> find_result = name_set.find(VariableName);
    if (!find_result.first) {
        if (parent_ == nullptr) return false;
        return parent_->check_id_safe(VariableName);
    }
    return true;
}

std::string IdentifierTIDS::element::check_id(const std::string& VariableName) {
    std::pair<bool, std::string&> find_result = name_set.find(VariableName);
    if (!find_result.first) {
        if (parent_ == nullptr) throw UndefinedVariable(VariableName);
        return parent_->check_id(VariableName);
    }
    return find_result.second;
}

cool_byte* IdentifierTIDS::element::find_id(const std::string &VariableName)
{
    std::pair<bool, cool_byte*> find_result = adr_name_set.find(VariableName);
    if (!find_result.first) {
        if (parent_ == nullptr) throw UndefinedVariable(VariableName);
        return parent_->find_id(VariableName);
    }
    return find_result.second;
}

size_t IdentifierTIDS::element::get_pointer_jump(const std::string &VariableName, StructTIDS& StrTIDS)
{
    if (!name_set.find(VariableName).first) {
        if (parent_ != nullptr) return parent_->get_pointer_jump(VariableName, StrTIDS);
        throw UndefinedVariable(VariableName);
    }
    return name_move.find(VariableName).second;
}

void IdentifierTIDS::element::__output_elem() const
{
    std::cout << "::\n";
    for (const Value& v : variables_) {
        std::cout << v.name() << " - " << v.type() << "\n";
    }
    std::cout << "::\n";
}

IdentifierTIDS::IdentifierTIDS()
    : cur_tid_(new element(nullptr)) {}

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

void IdentifierTIDS::__output_TID() const
{
    std::cout << "CUR_TID:\n----\n";
    auto v = cur_tid_;
    while (v) {
        v->__output_elem();
        v = v->parent_;
    }
    std::cout << "----\n";
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

void Function::check_func_par(int par_num, const std::string &type)
{
    if (par_num >= (int)argument_list_.size())
        throw TooManyParameters(name_,
                                argument_list_.size());
    if (argument_list_[par_num].type() != type)
        throw InvalidParameter(
            name_, argument_list_[par_num].type(),
            type);
}

void Function::check_param_count(const int &have_params)
{
    if (have_params < (int)argument_list_.size())
        throw TooFewParameters(
            name_, argument_list_.size(), have_params);
}

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

bool FunctionTIDS::check_func_id_safe(const std::string &func_name)
{
    return name_set.find(func_name).first;
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

Function* FunctionTIDS::getFunction(const std::string &name)
{
    std::pair<bool, Function&> find_result = name_set.find(name);
    if (!find_result.first) throw UndefinedFunction(name);
    return &name_set.find(name).second;
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
                         const Value& variable, int cnt) {
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    find_res.second.second.cur_tid()->push_id(variable, *this, cnt);
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

bool StructTIDS::check_id_safe(const std::string &struct_name, const std::string &variable_name)
{
    return name_set.find(struct_name).second.second.cur_tid()->check_id_safe(variable_name);
}

bool StructTIDS::check_func_id_safe(const std::string &struct_name, const std::string &func_name)
{
    return name_set.find(struct_name).second.first.check_func_id_safe(func_name);
}

size_t StructTIDS::get_pointer_jump(const std::string &struct_name, const std::string &member_name)
{
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    return find_res.second.second.cur_tid()->get_pointer_jump(member_name, *this);
}

size_t StructTIDS::get_func_poliz(const std::string &struct_name, const std::string &func_name)
{
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    return find_res.second.first.getFunction(func_name)->poliz_pos();
}

size_t StructTIDS::get_struct_size(const std::string &struct_name)
{
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    return find_res.second.second.cur_tid()->block_size;
}

Function *StructTIDS::getFunction(const std::string &struct_name, const std::string &func_name)
{
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>&> find_res =
        name_set.find(struct_name);
    if (!find_res.first) throw UndefinedStruct(struct_name);
    return find_res.second.first.getFunction(func_name);
}

void Value::set_name(const std::string& name) { name_ = name; }

void Value::set_type(const std::string& type) { type_ = type; }

std::string Value::get_type() { return type_; }

void Function::set_name(const std::string& name) { name_ = name; }

void Function::set_return_type(const std::string& type) { return_type_ = type; }
