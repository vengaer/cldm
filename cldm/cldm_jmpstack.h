#ifndef CLDM_JMPSTACK_H
#define CLDM_JMPSTACK_H

#include "cldm_macro.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>

enum { CLDM_JMPSTACK_STATIC_SIZE = 32 };

struct cldm_jmpstack {
    union {
        jmp_buf *stat[CLDM_JMPSTACK_STATIC_SIZE];
        jmp_buf **dyn;
    } s_un ;
    size_t size;
    size_t capacity;
};

#define cldm_jmpstack_init()                                                        \
    (struct cldm_jmpstack) {                                                        \
        .size = 0,                                                                  \
        .capacity = cldm_arrsize(cldm_declval(struct cldm_jmpstack).s_un.stat)      \
    }

inline jmp_buf **cldm_jmpstack_storage(struct cldm_jmpstack *stack) {
    return stack->capacity > cldm_arrsize(stack->s_un.stat) ? stack->s_un.dyn : stack->s_un.stat;
}

inline void cldm_jmpstack_free(struct cldm_jmpstack *stack) {
    if(stack->size > cldm_arrsize(stack->s_un.stat)) {
        free(stack->s_un.dyn);
    }
}

bool cldm_jmpstack_push(struct cldm_jmpstack *restrict stack, void *restrict addr);

inline jmp_buf *cldm_jmpstack_top(struct cldm_jmpstack *stack) {
    return cldm_jmpstack_storage(stack)[stack->size - 1];
}

inline void cldm_jmpstack_pop(struct cldm_jmpstack *stack) {
    --stack->size;
}

inline size_t cldm_jmpstack_size(struct cldm_jmpstack const *stack) {
    return stack->size;
}

inline size_t cldm_jmpstack_capacity(struct cldm_jmpstack const *stack) {
    return stack->capacity;
}

#endif /* CLDM_JMPSTACK_H */
