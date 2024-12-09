#ifndef LOOPLANG_STACK_HPP
#define LOOPLANG_STACK_HPP

#include "types.hpp"

struct Stack;

typedef size_t (*push_var_func_t) (Stack&);
extern const push_var_func_t __push_var[BASIC_TYPES_COUNT];

typedef void (*push_value_func_t) (Stack&, size_t);
extern const push_value_func_t __push_value[BASIC_TYPES_COUNT];

struct Stack {
    static constexpr const size_t start_size = 1024;
    size_t size = start_size;
    char* data = new char[start_size];
    size_t end = 0; // number of last free cell
    size_t var_end = 0;

    // по сути сами функции содержат в себе тип их аргументов

    // returns number of cell contains the value
    template <typename T> size_t push (T value) {
        *((T*)(data + end)) = value;
        end += sizeof(T);
        return end - sizeof(T); 
    }
    
    template <typename T> T pop () {
        end -= sizeof(T);
        return *((T*)(data + end));
    }

    template <typename T> T top() const {
        return *((T*)(data + end));
    }

    // True variables

    inline size_t push_var(Type type) {
        return __push_var[type](*this);
    }
    
    inline void push_value(size_t addr, Type type) {
        __push_value[type](*this, addr);
    }
};

// True variables func tables

/// PUSH_VAR
#define PUSH_VAR(__type)                                                \
size_t push_var_##__type (Stack& stack) {                               \
    stack.var_end = stack.end += sizeof(__type);                        \
    return stack.end - sizeof(__type);                                  \
}

PUSH_VAR(INT_T) PUSH_VAR(FLOAT_T) PUSH_VAR(CHAR_T) PUSH_VAR(STRING_T) 
PUSH_VAR(POINTER_T) PUSH_VAR(SIZE_T) PUSH_VAR(TYPE_T)

inline constexpr const push_var_func_t __push_var[BASIC_TYPES_COUNT] = {
    push_var_INT_T, push_var_FLOAT_T, push_var_CHAR_T, push_var_STRING_T,
    push_var_POINTER_T, push_var_SIZE_T, push_var_TYPE_T
};
#undef PUSH_VAR

/// PUSH_VALUE
#define PUSH_VALUE(__type)                                              \
void push_value_##__type (Stack& stack, size_t addr) {                  \
    *((__type*)(stack.data + stack.end)) = *((__type*)(stack.data + addr));\
    stack.end += sizeof(__type);                                        \
}

PUSH_VALUE(INT_T) PUSH_VALUE(FLOAT_T) PUSH_VALUE(CHAR_T) PUSH_VALUE(STRING_T)
PUSH_VALUE(POINTER_T) PUSH_VALUE(SIZE_T) PUSH_VALUE(TYPE_T)

inline constexpr const push_value_func_t __push_value[BASIC_TYPES_COUNT] = {
    push_value_INT_T, push_value_FLOAT_T, push_value_CHAR_T, push_value_STRING_T,
    push_value_POINTER_T, push_value_SIZE_T, push_value_TYPE_T
};
#undef PUSH_VALUE


#endif /* LOOPLANG_STACK_HPP */
