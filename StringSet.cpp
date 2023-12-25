#include "StringSet.h"
#include <cstring>


template<class T>
StringSet<T>::Vertex::Vertex(T value)
{
    std::memset(move_to, -1, sizeof(move_to));
    terminal = true;
    extra_value = value;
}

template<class T>
bool StringSet<T>::Vertex::terminate() { terminal = true; }

template<class T>
StringSet<T>::StringSet() {
    vertex_list.resize(1);
}

template<class T>
int StringSet<T>::add_vertex() {
    vertex_list.push_back(Vertex());
    return (int)vertex_list.size() - 1;
}

template<class T>
void StringSet<T>::insert(const std::string& str, T value = T()) {
    int cur_vert = 0;
    for (char c : str) {
        if (vertex_list[cur_vert].move_to[c] == -1) {
            vertex_list[cur_vert].move_to[c] = add_vertex();
        }
        cur_vert = vertex_list[cur_vert].move_to[c];
    }
    vertex_list[cur_vert].terminate();
    vertex_list[cur_vert].extra_value = value;
}

template<class T>
std::pair<bool, T> StringSet<T>::find(const std::string& str)
{
    int cur_vert = 0;
    for (char c : str) {
        if (vertex_list[cur_vert].move_to[c] == -1)
            return false;
        cur_vert = vertex_list[cur_vert].move_to[c];
    }
    return { vertex_list[cur_vert].terminal, vertex_list[cur_vert].extra_value };
}
