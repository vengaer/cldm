#include "avx2_memset_fuzz.h"

#include <cldm/cldm_avx2.h>

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int avx2_memset_fuzz(uint8_t const *data, size_t size) {
    bool crash;
    uint8_t *dst;

    if(!size) {
        return 0;
    }

    crash = true;

    dst = malloc(size * sizeof(*dst));
    if(!dst) {
        fputs("malloc failure\n", stderr);
        return 0;
    }

    if(cldm_avx2_memset(dst, data[0], size) != dst) {
        fputs("Invalid return value\n", stderr);
        goto epilogue;
    }

    crash = false;
    for(unsigned i = 0; i < size; i++) {
        if(dst[i] != data[0]) {
            fprintf(stderr, "Byte %u at address %p differs: dst[%u] = %" PRIu8 ", data = %" PRIu8 "\n", i, (void *)&dst[i], i, dst[i], data[0]);
            crash = true;
        }
    }

epilogue:
    free(dst);
    if(crash) {
        abort();
    }
    return 0;
}
