#pragma once
#include <string>
#include <vector>
#include "uni_ptr.cpp"

#include "StringSet.cpp"

class StructTIDS;
class IdentifierTIDS;
class FunctionTIDS;

//**
Стуктура, обозначающая переменную в формате тип-имя
*//
struct Value {
    std::string type_;
    std::string name_;

    Value();

    Value(const std::string& type, const std::string& name);

    void set_name(const std::string& name);
    void set_type(const std::string& type);
    std::string get_type();

    std::string name() const;
    std::string type() const;

    bool operator==(const Value& other) const;
};

size_t get_type_size(const std::string& str, StructTIDS& StrTIDS);

bool duplicates(Value& first, Value& second);

//**
Тиды идентификаторов
*//
class IdentifierTIDS {
   public:
    struct element {
        StringSet<std::string> name_set;
        StringSet<cool_byte*> adr_name_set;
        size_t block_size = 0;

        element* parent_;
        std::vector<Value> variables_;

        ~element();

        element(element* parent = nullptr);
        // добавляет переменную в тид, cnt != 1, если переменная - массив
        void push_id(const Value& Variable, StructTIDS& StrTIDS, int cnt = 1);

        // возвращает, есть ли этот элемент в тиде. не кидает ошибку, если его нет
        bool check_id_safe(const std::string& VariableName);
        // возвращает тип переменной
        std::string check_id(const std::string& VariableName);
        // возвращает блок памяти,принадлежащий этой переменной
        cool_byte*& find_id(const std::string& VariableName);
        // метод для структур, возвращает позицию в блоке памяти, которую занимает переменная VariableName в идентификационном тиде структуры 
        size_t get_pointer_jump(const std::string& VariableName, StructTIDS& StrTIDS);
        // показывает содержимое тида
        void __output_elem() const;
    };

    IdentifierTIDS();

    element*& cur_tid();

    void create_TID();

    void del_TID();
    
    bool is_empty() const;

    void __output_TID() const;

   private:
    element* cur_tid_;
};

// структура содержит информацию о конкретной функции с данными параметрами
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
    // проверяет соответствие параметра функции под номером par_num и type
    void check_func_par(int par_num, const std::string& type);
    
    void check_param_count(const int& have_params);
};

bool duplicates(Function& F1, Function F2);

// класс, описывающий тид для функций
class FunctionTIDS {
   public:
    FunctionTIDS();

    void push_func_id(const std::string& func_name);

    bool check_func_id_safe(const std::string& func_name);

    std::string check_func_id(const std::string& func_name);

    void check_func_par(const std::string& func_name, int par_num,
                        const std::string& type);

    void push_func_par(const std::string& func_name, const Value& variable);

    void push_func_return_type(const std::string& func_name,
                               const std::string& return_type);
    // для функции func_name установить начало полиза, равное pos
    void push_func_poliz_pos(const std::string& func_name,
                             const std::size_t& pos);

    void check_param_count(const std::string& func_name,
                           const int& have_params);

    void check_exist_id(const std::string& func_name);
    // возвращает указатель на объект функции
    Function* getFunction(const std::string& func_name);

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

    bool check_id_safe(const std::string& struct_name,
                       const std::string& variable_name);

    bool check_func_id_safe(const std::string& struct_name,
                            const std::string& func_name);

    size_t get_pointer_jump(const std::string& struct_name,
                            const std::string& member_name);

    size_t get_func_poliz(const std::string& struct_name,
                          const std::string& func_name);
    
    size_t get_struct_size(const std::string& struct_name);

    Function* getFunction(const std::string& struct_name,
                         const std::string& func_name);

   private:
    StringSet<std::pair<FunctionTIDS, IdentifierTIDS>> name_set;
};
