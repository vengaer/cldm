#ifndef MEMORY_UTILS_H
#define MEMORY_UTILS_H

#include <cldm/cldm_config.h>

#include <ctype.h>
#include <stddef.h>
#include <stdint.h>

inline size_t alignment(void const *addr) {
    return (size_t)addr & -(size_t)addr;
}

inline size_t pgdistance(void const *addr) {
    return CLDM_PGSIZE - ((size_t)addr & (CLDM_PGSIZE - 1));
}

inline char asciicvt(uint8_t byte) {
    return isprint(byte) ? (char)byte : (char)(((unsigned char)byte & ('~' - ' ')) + ' ');
}


#endif /* MEMORY_UTILS_H */
