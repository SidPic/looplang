#ifndef LOOPLANG_BASIC_FUNCTIONS
#define LOOPLANG_BASIC_FUNCTIONS

#include "stack.hpp"
#include <iostream>
/// ASSIGN
#define BF_ASSIGN(__type)                                               \
void bf_assign_##__type(Stack& stack) {                                 \
    size_t addr = stack.pop<size_t>();                                  \
    *((__type*)(stack.data + addr)) = stack.pop<__type>();              \
    std::cout << "value: " << *((__type*)(stack.data + addr)) << std::endl;\
}                                                       

BF_ASSIGN(INT_T) BF_ASSIGN(FLOAT_T) BF_ASSIGN(CHAR_T) BF_ASSIGN(STRING_T)
BF_ASSIGN(POINTER_T) BF_ASSIGN(SIZE_T) BF_ASSIGN(TYPE_T)

inline constexpr const func_t bf_assign[BASIC_TYPES_COUNT] = {
    bf_assign_INT_T, bf_assign_FLOAT_T, bf_assign_CHAR_T, bf_assign_STRING_T,
    bf_assign_POINTER_T, bf_assign_SIZE_T, bf_assign_TYPE_T
};
#undef BF_ASSIGN




#endif /* LOOPLANG_BASIC_FUNCTIONS */
