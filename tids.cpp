#include "tids.h"

// struct Value

Value::Value()
    : type("")
    , name("")
{}

Value::Value(const std::string & type, const std::string& name)
    : type_(type)
    , name_(name)
{}

std::string& Value::name()
{
    return name_;
}

std::string& Value::type()
{
    return type_;
}

bool Value::operator==(const Value& other) const
{
    return other.type() == type() && other.name() == name();
}

bool duplicates(const Value& first, const Value& second)
{
    return first.name() == second.name();
}

// class IdentifierTIDS

IdentifierTIDS::element::element(const element* parent = nullptr)
    : parent_(parent_)
    , children_({})
    , variables_({})
{}

void IdentifierTIDS::element::push_id(const Value & Value)
{
    if (name_set.find(Value.name_).first)
        throw IdentifierAlreadyDefined(Value.name());
    values_.push_back(Value);
    name_set.insert(Value.name_, Value.type_);
}

std::string IdentifierTIDS::element::check_id(const std::string& VariableName)
{
    std::pair<bool, std::string> find_result = name_set.find(VariableName);
    if (!find_result.first) {
        if (parent == nullptr)
            throw UndefinedVariable(VariableName);
        return parent->check_id(VariableName);
    }
    return find_result.second;
}

IdentifierTIDS::IdentifierTID()
    : cur_tid_(new element(nullptr))
{}

element*& IdentifierTIDS::cur_tid()
{
    return cur_tid_;
}

void IdentifierTIDS::create_TID()
{
    cur_tid_->children_.push_back(new element(cur_tid_));
    cur_tid_ = cur_tid_->children_.back();
}

void IdentifierTIDS::del_TID()
{
    element* q = cur_tid_;
    cur_tid_ = cur_tid->parent_;
    delete q;
}

// struct Function

bool duplicates(const Function& F1, const Function F2)
{
    if (!F1.name() != !F2.name())
        return false;
    if (!F1.argument_list() != !F2.argument_list())
        return false;
    return true;
}

Function::Function(std::string name) : name_(name) {}

const std::string& Function::return_type() const
{
    return return_type_;
}

const std::string& Function::name() const
{
    return name_;
}

const std::vector<Value>& Function::argument_list() const
{
    return argument_list_;
}

// class FunctionTIDS

FunctionTIDS::FunctionTIDS() {}

void FunctionTIDS::push_func_id(const std::string& func_name)
{
    std::pair<bool, Function> find_result = name_set.find(func_name);
    if (find_result.first)
        throw FunctionAlreadyDefined(func_name);
    name_set.insert(func_name, Function(func_name));
}

std::string FunctionTIDS::check_func_id(const std::string& func_name)
{
    std::pair<bool, Function> find_result = name_set.find(func_name);
    if (!find_result.first)
        throw UndefinedFunction();
    return find_result.second.return_type();
}

void FunctionTIDS::push_func_return_type(const std::string& func_name, const std::string& return_type)
{
    std::pair<bool, Function> find_result = name_set.find(func_name);
    if (!find_result.first)
        throw UndefinedFunction();
    find_result.second.set_return_type(return_type);
}

void FunctionTIDS::check_param_count(const std::string& func_name, const int& have_params)
{
    std::pair<bool, Function> find_result = name_set.find(func_name);
    if (!func_result.first)
        throw UndefinedFunction();
    if (par_num < (int)find_result.second.argument_list().size())
        throw TooFewParameters(func_name, find_result.second.argument_list().size(), par_num);
}

void FunctionTIDS::check_func_par(const std::string& func_name, int par_num, const std::string& type)
{
    std::pair<bool, Function> find_result = name_set.find(func_name);
    if (!func_result.first)
        throw UndefinedFunction();
    if (par_num >= (int)find_result.second.argument_list().size())
        throw TooManyParameters(func_name, (int)find_result.second.argument_list().size());
    if (find_result.second.argument_list()[par_num].type() != type)
        throw InvalidParameter(func_name, find_result.second.argument_list()[par_num].type(), type);
}

void FunctionTIDS::push_func_par(const std::string& func_name, const Value& variable)
{
    std::pair<bool, Function> find_res = name_set.find(func_name);
    if (!func_result.first)
        throw UndefinedFunction();
    Function& fun = find_res.second;
    std::pair<bool, int> par_num = fun.find(variable.name());
    if (par_num.first)
        throw IdentifierAlreadyDefined();
    fun.insert(variable.name(), fun.argument_list().size());
    fun.argument_list().push_back(variable);
}

// class StructTIDS

StructTIDS::StructTIDS() {}


void StructTIDS::push_id(const std::string& struct_name, const std::string& name)
{   
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>> find_res = name_set.find(struct_name);
    if (!find_res.first)
        throw UndefinedStruct();
    find_res.second.second.cur_tid()->push_id(name);
}

std::string StructTIDS::check_id(const std::string& struct_name, const std::string& name)
{
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>> find_res = name_set.find(struct_name);
    if (!find_res.first)
        throw UndefinedStruct();
    return find_res.second.second.cur_tid()->check_id(name);
}

void StructTIDS::push_struct_id(const std::string& struct_name)
{
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>> find_res = name_set.find(struct_name);
    if (find_res.first)
        throw StructAlreadyDefined();
    name_set.insert(struct_name);
}

void StructTIDS::check_struct_id(const std::string& struct_name)
{
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>> find_res = name_set.find(struct_name);
    if (!find_res.first)
        throw UndefinedStruct();
}

void StructTIDS::push_func_id(const std::string& struct_name, const std::string& func_name)
{
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>> find_res = name_set.find(struct_name);
    if (!find_res.first)
        throw UndefinedStruct();
    find_res.second.first.push_func_id(func_name);
}

std::string StructTIDS::check_func_id(const std::string& struct_name, const std::string& func_name)
{
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>> find_res = name_set.find(struct_name);
    if (!find_res.first)
        throw UndefinedStruct();
    return find_res.second.first.check_func_id(func_name);
}

void StructTIDS::push_func_return_type(const std::string& struct_name, const std::string& func_name, const std::string& return_type)
{
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>> find_res = name_set.find(struct_name);
    if (!find_res.first)
        throw UndefinedStruct();
    find_res.second.first.push_func_return_type(func_name, return_type);
}

void StructTIDS::check_func_par(const std::string& struct_name, const std::string& func_name, int par_num, const std::string& type)
{
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>> find_res = name_set.find(struct_name);
    if (!find_res.first)
        throw UndefinedStruct();
    find_res.second.first.check_func_par(func_name, par_num, type);
}

void StructTIDS::push_func_par(const std::string& struct_name, const std::string& func_name, const Value& variable)
{
    std::pair<bool, std::pair<FunctionTIDS, IdentifierTIDS>> find_res = name_set.find(struct_name);
    if (!find_res.first)
        throw UndefinedStruct();
    find_res.second.first.push_func_par(func_name, variable)
}

void Value::set_name(const std::string& name)
{
    name_ = name;
}

void Value::set_type(const std::string& type)
{
    type_ = type;
}

void Function::set_name(const std::string& name)
{
    name_ = name;
}

void Function::set_return_type(const std::string& type)
{
    return_type_ = type;
}
