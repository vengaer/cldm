#include "cldm_byteseq.h"
#include "cldm_jmpstack.h"
#include "cldm_log.h"

jmp_buf **cldm_jmpstack_storage(struct cldm_jmpstack *stack);
jmp_buf *cldm_jmpstack_top(struct cldm_jmpstack *stack);
void cldm_jmpstack_free(struct cldm_jmpstack *stack);
void cldm_jmpstack_pop(struct cldm_jmpstack *stack);
size_t cldm_jmpstack_size(struct cldm_jmpstack const *stack);
size_t cldm_jmpstack_capacity(struct cldm_jmpstack const *stack);

bool cldm_jmpstack_push(struct cldm_jmpstack *restrict stack, void *restrict jmpbuf) {
    void *buf;
    if(stack->size == stack->capacity) {
        if(stack->capacity == cldm_arrsize(stack->s_un.stat)) {
            buf = malloc((stack->capacity << 1u) * sizeof(*stack->s_un.dyn));
            if(!buf) {
                cldm_err("Could not allocate jump stack");
                return false;
            }
            cldm_memcpy(buf, stack->s_un.stat, sizeof(stack->s_un.stat));
        }
        else {
            buf = realloc(stack->s_un.dyn, (stack->capacity << 1u) * sizeof(*stack->s_un.dyn));
            if(!buf) {
                cldm_err("Could not allocate jump stack");
                free(stack->s_un.dyn);
                return false;
            }
        }
        stack->s_un.dyn = buf;
        stack->capacity <<= 1u;
    }
    cldm_jmpstack_storage(stack)[stack->size++] = jmpbuf;
    return true;
}
