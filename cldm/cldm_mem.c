#include "cldm_mem.h"

#define cldm_copy_bytes(dst, src, size)                                                     \
    union dmemacc d = {                                                                     \
        .byte = dst                                                                         \
    };                                                                                      \
    union smemacc s = {                                                                     \
        .byte = src                                                                         \
    };                                                                                      \
                                                                                            \
    size_t align = cldm_alignof(unsigned);                                                  \
    unsigned i = 0;                                                                         \
                                                                                            \
    for(; i < size && ((((size_t)d.byte) % align) || (((size_t)s.byte) % align)); i++) {    \
        *d.byte++ = *s.byte++;                                                              \
    }                                                                                       \
    for(; i + sizeof(*s.dword) <= size; i += sizeof(*s.dword)) {                            \
        *d.dword++ = *s.dword++;                                                            \
    }                                                                                       \
    for(; i < size; i++) {                                                                  \
        *d.byte++ = *s.byte++;                                                              \
    }                                                                                       \
                                                                                            \
    return dst

union dmemacc {
    unsigned char *byte;
    unsigned *dword;
};

union smemacc {
    unsigned char const* byte;
    unsigned const *dword;
};

void *cldm_mmove(void *dst, void const* src, size_t size) {
    cldm_copy_bytes(dst, src, size);
}

void *cldm_mcpy(void *restrict dst, void const *restrict src, size_t size) {
    cldm_copy_bytes(dst, src, size);
}
