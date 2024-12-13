#ifndef LOOPLANG_STACK_HPP
#define LOOPLANG_STACK_HPP

#include "types.hpp"

struct Stack;

typedef size_t (*push_var_func_t) (Stack&);
extern const push_var_func_t __push_var[BASIC_TYPES_COUNT];

typedef void (*push_value_func_t) (Stack&, size_t);
extern const push_value_func_t __push_value[BASIC_TYPES_COUNT];

typedef void (*push_literal_func_t) (Stack&, Value);
extern const push_literal_func_t __push_literal[BASIC_TYPES_COUNT];

struct Stack {
    static constexpr const size_t start_size = 1024;
    size_t size = start_size;
    char* data = new char[start_size];
    size_t end = 0; // number of last free cell
    size_t var_end = 0;
    Type top_expr_type;

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
        return *((T*)(data + end - sizeof(T)));
    }

    // True variables

    inline size_t push_var(Type type) {
        return __push_var[type](*this);
    }

    inline void push_value(TypedValue tval) {
        __push_value[tval.type](*this, tval.VAL_SIZE_T);
    }

    inline void push_literal(TypedValue tval) {
        __push_literal[tval.type](*this, tval.value);
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

/// PUSH_LITERAL
#define PUSH_LITERAL(__type)                                            \
void push_literal_##__type (Stack& stack, Value value) {                \
    *((__type*)(stack.data + stack.end)) = (__type)value;               \
    stack.end += sizeof(__type);                                        \
}

PUSH_LITERAL(INT_T) PUSH_LITERAL(FLOAT_T) PUSH_LITERAL(CHAR_T) PUSH_LITERAL(STRING_T)
PUSH_LITERAL(POINTER_T) PUSH_LITERAL(SIZE_T) PUSH_LITERAL(TYPE_T)

inline constexpr const push_literal_func_t __push_literal[BASIC_TYPES_COUNT] = {
    push_literal_INT_T, push_literal_FLOAT_T, push_literal_CHAR_T, push_literal_STRING_T,
    push_literal_POINTER_T, push_literal_SIZE_T, push_literal_TYPE_T
};
#undef PUSH_LITERAL


#endif /* LOOPLANG_STACK_HPP */
