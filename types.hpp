#ifndef LOOPLANG_TYPES
#define LOOPLANG_TYPES

#include <cstdint>
#include <variant>

typedef unsigned char Type;
enum : unsigned char {
    INT_TYPE, FLOAT_TYPE, CHAR_TYPE,
    STRING_TYPE, POINTER_TYPE, SIZE_TYPE,
    TYPE_TYPE, BASIC_TYPES_COUNT,

    VOID_TYPE = BASIC_TYPES_COUNT,
    VALUE_TYPE, FUNC_TYPE
};

enum ext_t { VAR_EXT, FUNC_EXT, LITERAL_EXT, NULL_EXT };

typedef int INT_T;
typedef double FLOAT_T;
typedef char CHAR_T;
typedef char* STRING_T;
typedef void* POINTER_T;
typedef size_t SIZE_T;
typedef Type TYPE_T;

struct Stack;
typedef void (*func_t) (Stack& stack);
typedef func_t FUNC_T;


#include <iostream>

union Value {
    #define ADD_TYPE(T)                                                 \
    T VAL_##T;                                                          \
    Value (const T& value): VAL_##T (value) {}                          \
    T& operator= (const T& value) {                                     \
        return VAL_##T = value;                                         \
    }                                                                   \
    operator T () const { return VAL_##T; }

    ADD_TYPE(INT_T) ADD_TYPE(CHAR_T) ADD_TYPE(FLOAT_T)
    ADD_TYPE(STRING_T) ADD_TYPE(POINTER_T) ADD_TYPE(SIZE_T)
    ADD_TYPE(TYPE_T) ADD_TYPE(FUNC_T)
    #undef ADD_TYPE
};
typedef Value VALUE_T;

struct TypedValue {
    union {
        #define ADD_TYPE(T) T VAL_##T;
        ADD_TYPE(INT_T) ADD_TYPE(CHAR_T) ADD_TYPE(FLOAT_T)
        ADD_TYPE(STRING_T) ADD_TYPE(POINTER_T) ADD_TYPE(SIZE_T)
        ADD_TYPE(TYPE_T) ADD_TYPE(FUNC_T)
        ADD_TYPE(VALUE_T)
        #undef ADD_TYPE

        VALUE_T value;
    };

    struct {
        unsigned type : 6;
        unsigned extension : 2;
    };

    #define ADD_TYPE(T)                                                             \
    TypedValue (const T& __value, unsigned ext = LITERAL_EXT, Type __type=T##YPE):  \
        VAL_##T (__value), type(__type), extension(ext)                             \
    {}                                                                              \
    T& operator= (const T& __value) {                               \
        type = T##YPE;                                              \
        extension = LITERAL_EXT;                                    \
        return VAL_##T = __value;                                   \
    }

    ADD_TYPE(INT_T) ADD_TYPE(CHAR_T) ADD_TYPE(FLOAT_T)
    ADD_TYPE(STRING_T) ADD_TYPE(POINTER_T) ADD_TYPE(SIZE_T)
    ADD_TYPE(TYPE_T) ADD_TYPE(FUNC_T)
    ADD_TYPE(VALUE_T)
    #undef ADD_TYPE
};

#endif /* LOOPLANG_TYPES */
