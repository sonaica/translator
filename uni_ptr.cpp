#pragma once

typedef int64_t cool_byte;

struct uni_ptr {
    cool_byte* ptr = nullptr;
    bool deleteable = false;

    uni_ptr() {
        ptr = nullptr;
    }
    void create() {
        ptr = new cool_byte;
    }
    uni_ptr(bool deleteable_) {
        if (deleteable) {
            ptr = new cool_byte;
        }
        deleteable = deleteable_;
    }
    ~uni_ptr() {
        if (deleteable) {
            delete ptr;
            ptr = nullptr;
        }
    }
    void switch_deleteable() {
        deleteable ^= 1;
    }
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
        swap(deleteable, other.deleteable);
    }

    uni_ptr& operator=(uni_ptr other) {
        swap(other);
        return *this;
    }
};