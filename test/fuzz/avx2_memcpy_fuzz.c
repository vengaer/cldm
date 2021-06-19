#include "avx2_memcpy_fuzz.h"
#include "memory_utils.h"

#include <cldm/cldm_config.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void *cldm_avx2_memcpy(void *restrict dst, void const *restrict src, unsigned long long n);

static void dump(uint8_t const *restrict dst, uint8_t const *restrict data, size_t size) {
    hexdump("dst", dst, size, stderr);
    hexdump("data", data, size, stderr);
    for(unsigned i = 0; i < size; i++) {
        if(dst[i] != data[i]) {
            fprintf(stderr, "Offending byte at offset %u\n", i);
            fprintf(stderr, "%-*s^\n", (int)6 * i + 4, "");
        }
    }
}

int avx2_memcpy_fuzz(uint8_t const *data, size_t size) {
    uint8_t *dst;
    bool crash;

    if(!size) {
        return 0;
    }

    crash = true;

    dst = malloc(size * sizeof(uint8_t));
    if(!dst) {
        fputs("malloc failure\n", stderr);
        return 0;
    }

    if(cldm_avx2_memcpy(dst, data, size) != dst) {
        fputs("Incorrect return value\n", stderr);
        goto epilogue;
    }

    if(memcmp(dst, data, size)) {
        fputs("Error on copy\n", stderr);
        dump(dst, data, size);
        goto epilogue;
    }

    crash = false;
epilogue:
    free(dst);
    if(crash) {
        abort();
    }
    return 0;
}
