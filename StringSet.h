#pragma once
#include <vector>
#include <string>

template<class T>
class StringSet {
    
    struct Vertex {

        static const int ALPHABET_SIZE = 1 << 8;
        
        int move_to[ALPHABET_SIZE];
        bool terminal;
        T extra_value;

        Vertex(T value = T());

        ~Vertex();

        void terminate();

        bool is_terminal() const;
    };

    std::vector<Vertex> vertex_list;

    int add_vertex();

public:

    StringSet();

    void insert(const std::string& str, T value = T());

    std::pair<bool,T&> find(const std::string& str);

    T& operator[](const std::string& str);
};

template <class T>
class StringSet<T*> {
    
    struct Vertex {

        static const int ALPHABET_SIZE = 1 << 8;
        
        int move_to[ALPHABET_SIZE];
        bool terminal;
        T* extra_value;

        Vertex(T* value = nullptr);

        void terminate();

        bool is_terminal() const;
    };

    std::vector<Vertex> vertex_list;

    int add_vertex();

public:

    StringSet();

    ~StringSet();

    void insert(const std::string& str, T* value = nullptr);

    std::pair<bool,T*> find(const std::string& str);
};