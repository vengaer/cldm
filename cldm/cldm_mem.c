#include "cldm_mem.h"


void *cldm_mmove(void *dst, void const* src, size_t size) {
    unsigned char *d = dst;
    unsigned char const *s = src;
    while(size--) {
        *(unsigned char *)d++ = *(unsigned char *)s++;
    }
    return dst;
}

void *cldm_mcpy(void *restrict dst, void const *restrict src, size_t size) {
    union {
        unsigned char *byte;
        unsigned *dword;
    } d = { .byte = dst };
    union {
        unsigned char const *byte;
        unsigned const *dword;
    } s = { .byte = src };

    size_t align = cldm_alignof(unsigned);
    unsigned i = 0;

    for(; i < size && ((((size_t)d.byte) % align) || (((size_t)s.byte) % align)); i++) {
        *d.byte++ = *s.byte++;
    }
    for(; i + sizeof(*s.dword) <= size; i += sizeof(*s.dword)) {
        *d.dword++ = *s.dword++;
    }
    for(; i < size; i++) {
        *d.byte++ = *s.byte++;
    }

    return dst;
}
