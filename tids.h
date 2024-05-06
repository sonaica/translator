#pragma once
#include <string>
#include <vector>
#include "uni_ptr.cpp"

#include "StringSet.cpp"

struct Value {
    std::string type_;
    std::string name_;
    uni_ptr ptr;

    Value();

    Value(const std::string& type, const std::string& name);

    void set_name(const std::string& name);
    void set_type(const std::string& type);
    std::string get_type();

    std::string name() const;
    std::string type() const;

    bool operator==(const Value& other) const;
};

bool duplicates(Value& first, Value& second);

class IdentifierTIDS {
   public:
    struct element {
        StringSet<std::string> name_set;
        StringSet<cool_byte*> adr_name_set;

        element* parent_;
        std::vector<Value> variables_;

        ~element();

        element(element* parent = nullptr);

        void push_id(const Value& Variable);

        std::string check_id(const std::string& VariableName);

        cool_byte*& find_id(const std::string& VariableName);
    };

    IdentifierTIDS();

    element*& cur_tid();

    void create_TID();

    void del_TID();
    
    bool is_empty() const;

   private:
    element* cur_tid_;
};

struct Function {
    StringSet<int> name_set;

    std::string name_;
    std::string return_type_;
    std::vector<Value> argument_list_;
    size_t poliz_pos_;

    void set_name(const std::string& name);
    void set_return_type(const std::string& type);

    Function(std::string name = "", size_t poliz_pos = -1);

    const std::string& return_type() const;

    const std::string& name() const;

    size_t poliz_pos() const;
    void set_poliz_pos(const size_t& new_pos);

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

    void push_func_poliz_pos(const std::string& func_name,
                             const std::size_t& pos);

    void check_param_count(const std::string& func_name,
                           const int& have_params);

    void check_exist_id(const std::string& func_name);

    Function& getFunction(const std::string& func_name);

   private:
    StringSet<Function> name_set;
};

class StructTIDS {
   public:
    StructTIDS();

    void push_id(const std::string& struct_name, const Value& name);

    bool check_struct_existance(const std::string& struct_name);

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

    void push_func_poliz_pos(const std::string& struct_name,
                             const std::string& func_name,
                             const std::size_t& pos);

    void check_param_count(const std::string& struct_name,
                           const std::string& func_name,
                           const int& have_params);

   private:
    StringSet<std::pair<FunctionTIDS, IdentifierTIDS>> name_set;
};
