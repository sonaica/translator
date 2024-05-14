#pragma once
#include "StringSet.h"
#include <cstring>

//**
Словарь, который работает по принципу бора. Ставит в соответствие строку к значению типа Т
*//
template<class T>
StringSet<T>::Vertex::Vertex(T value)
{
    std::memset(move_to, -1, sizeof(move_to));
    terminal = false;
    extra_value = value;
}

//**
Делает вершину терминальной
*//
template<class T>
void StringSet<T>::Vertex::terminate() { terminal = true; }

//**
Возвращает "статус" вершины (терминальная/нетерминальная)
*//
template <class T>
bool StringSet<T>::Vertex::is_terminal() const
{
    return terminal;
}

//**
Конструктор, создающий базовую вершину
*//
template<class T>
StringSet<T>::StringSet() {
    vertex_list.resize(1);
}

//**
Добавляет вершину в бор
*//
template<class T>
int StringSet<T>::add_vertex() {
    vertex_list.push_back(Vertex());
    return (int)vertex_list.size() - 1;
}

//**
Вставляет в бор элемент со значением value по ключу str
*//
template<class T>
void StringSet<T>::insert(const std::string& str, T value) {
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

//**
Определяет, существует ли элемент с ключом str, и, если есть, возвращает ссылку на этот элемент.
*//
template<class T>
std::pair<bool, T&> StringSet<T>::find(const std::string& str)
{
    int cur_vert = 0;
    for (char c : str) {
        T temp;
        if (vertex_list[cur_vert].move_to[c] == -1)
            return {false, temp};
        cur_vert = vertex_list[cur_vert].move_to[c];
    }
    return { vertex_list[cur_vert].terminal, vertex_list[cur_vert].extra_value };
}


//**
убрать
*//
template <class T>
T &StringSet<T>::operator[](const std::string &str)
{
    return find(str).second;
}
