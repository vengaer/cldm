#ifndef MEMORY_UTILS_H
#define MEMORY_UTILS_H

#include <cldm/cldm_config.h>

#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

inline size_t alignment(void const *addr) {
    return (size_t)addr & -(size_t)addr;
}

inline size_t pgdistance(void const *addr) {
    return CLDM_PGSIZE - ((size_t)addr & (CLDM_PGSIZE - 1));
}

inline char asciicvt(uint8_t byte) {
    return isprint(byte) ? (char)byte : (char)(((unsigned char)byte % ('~' - ' ')) + ' ');
}

inline void hexdump(char const *restrict bufname, uint8_t const *restrict data, size_t size, FILE *restrict fp) {
    fprintf(fp, "%s:\n  { ", bufname);
    for(unsigned i = 0; i < size; i++) {
        fprintf(fp, "0x%02x, ", data[i]);
    }
    fprintf(fp, "\b\b }\n  Length: %zu\n  Alignment: %zu\n", size, alignment(data));
}


#endif /* MEMORY_UTILS_H */
