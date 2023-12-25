#pragma once
#include <string>
#include <vector>

#include "StringSet.cpp"

struct Value {
    std::string type_;
    std::string name_;

    Value();

    Value(const std::string& type, const std::string& name);

    void set_name(const std::string& name);
    void set_type(const std::string& type);

    std::string name() const;
    std::string type() const;

    bool operator==(const Value& other) const;
};

bool duplicates(Value& first, Value& second);

class IdentifierTIDS {
   public:
    struct element {
        StringSet<std::string> name_set;

        element* parent_;
        std::vector<element*> children_;
        std::vector<Value> variables_;

        element(element* parent = nullptr);

        void push_id(const Value& Variable);

        std::string check_id(const std::string& VariableName);
    };

    IdentifierTIDS();

    element*& cur_tid();

    void create_TID();

    void del_TID();

   private:
    element* cur_tid_;
};

struct Function {
    StringSet<int> name_set;

    std::string name_;
    std::string return_type_;
    std::vector<Value> argument_list_;

    void set_name(const std::string& name);
    void set_return_type(const std::string& type);

    Function(std::string name = "");

    const std::string& return_type() const;

    const std::string& name() const;

    std::vector<Value>& argument_list();
};

bool duplicates(Function& F1, Function F2);

class FunctionTIDS {
   public:
    FunctionTIDS();

    void push_func_id(const std::string& func_name);

    std::string check_func_id(const std::string& func_name);

    void check_func_par(const std::string& func_name, int par_num,
                        const std::string& type);

    void push_func_par(const std::string& func_name, const Value& variable);

    void push_func_return_type(const std::string& func_name,
                               const std::string& return_type);

    void check_param_count(const std::string& func_name,
                           const int& have_params);

   private:
    StringSet<Function> name_set;
};

class StructTIDS {
   public:
    StructTIDS();

    void push_id(const std::string& struct_name, const Value& name);

    std::string check_id(const std::string& struct_name,
                         const std::string& name);

    void push_struct_id(const std::string& struct_name);

    void check_struct_id(const std::string& struct_name);

    void push_func_id(const std::string& struct_name,
                      const std::string& func_name);

    std::string check_func_id(const std::string& struct_name,
                              const std::string& func_name);

    void check_func_par(const std::string& struct_name,
                        const std::string& func_name, int par_num,
                        const std::string& type);

    void push_func_par(const std::string& struct_name,
                       const std::string& func_name, const Value& variable);

    void push_func_return_type(const std::string& struct_name,
                               const std::string& func_name,
                               const std::string& return_type);

    void check_param_count(const std::string& struct_name,
                           const std::string& func_name,
                           const int& have_params);

   private:
    StringSet<std::pair<FunctionTIDS, IdentifierTIDS>> name_set;
};
