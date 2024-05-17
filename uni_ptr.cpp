#pragma once

typedef char cool_byte;

struct uni_ptr {
    cool_byte* ptr = nullptr;

    uni_ptr() {
        ptr = nullptr;
    }
    void create(size_t sz) {
        if (ptr != nullptr)
            delete[] ptr;
        ptr = new cool_byte[sz];
    }
    ~uni_ptr() {}
    int64_t* int_ptr() const {
        return (int64_t*)ptr;
    }
    double* double_ptr() const {
        return (double*)ptr;
    }
    bool* bool_ptr() const {
        return (bool*)ptr;
    }
    int64_t& get_int() {
        return *(int64_t*)ptr;
    }
    double& get_double() {
        return *(double*)ptr;
    }
    bool& get_bool() {
        return *(bool*)ptr;
    }
    bool is_null() const {
        return ptr == nullptr;
    }

    void swap(uni_ptr& other) {
        using std::swap;
        swap(ptr, other.ptr);
    }

    uni_ptr& operator=(uni_ptr other) {
        swap(other);
        return *this;
    }
};