#ifndef LOOPLANG_TYPES
#define LOOPLANG_TYPES

enum Type : unsigned char {
    INT_TYPE, FLOAT_TYPE, CHAR_TYPE,
    STRING_TYPE, POINTER_TYPE, SIZE_TYPE,
    TYPE_TYPE, BASIC_TYPES_COUNT
};

typedef int INT_T;
typedef double FLOAT_T;
typedef char CHAR_T;
typedef char* STRING_T;
typedef void* POINTER_T;
typedef size_t SIZE_T;
typedef Type TYPE_T;


// BASIC FUNCTIONS
#include "stack.hpp"

typedef void (*func_t) (Stack& stack);

/// ASSIGN
#define BF_ASSIGN(__type)                                               \
void bf_assign_##__type(Stack& stack) {                                 \
    size_t addr = stack.pop<size_t>();                                  \
    *((__type*)(stack.data + addr)) = stack.pop<__type>();              \
}                                                       

BF_ASSIGN(INT_T) BF_ASSIGN(FLOAT_T) BF_ASSIGN(CHAR_T) BF_ASSIGN(STRING_T)
BF_ASSIGN(POINTER_T) BF_ASSIGN(SIZE_T) BF_ASSIGN(TYPE_T)

inline constexpr const func_t bf_assign[BASIC_TYPES_COUNT] = {
    bf_assign_INT_T, bf_assign_FLOAT_T, bf_assign_CHAR_T, bf_assign_STRING_T,
    bf_assign_POINTER_T, bf_assign_SIZE_T, bf_assign_TYPE_T
};
#undef BF_ASSIGN



#endif /* LOOPLANG_TYPES */
