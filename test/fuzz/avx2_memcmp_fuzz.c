#include "avx2_memcmp_fuzz.h"
#include "memory_utils.h"

#include <cldm/cldm_avx2.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int avx2_memcmp_fuzz(uint8_t const *data, size_t size) {
    bool crash;
    int rv;
    uint8_t *dst;
    unsigned idx;

    if(!size) {
        return 0;
    }

    crash = true;

    dst = malloc(size * sizeof(*dst));
    if(!dst) {
        fputs("malloc failure\n", stderr);
        return 0;
    }

    memcpy(dst, data, size);

    rv = cldm_avx2_memcmp(dst, data, size);
    if(rv) {
        fprintf(stderr, "%d: Expected zero but got %d\n", __LINE__, rv);
        dump(dst, data, size);
        goto epilogue;
    }

    rv = cldm_avx2_memcmp(data, dst, size);
    if(rv) {
        fprintf(stderr, "%d: Expected zero but got %d\n", __LINE__, rv);
        dump(dst, data, size);
        goto epilogue;
    }

    idx = (unsigned)(((double)data[0] / UINT8_MAX) * (double)(size - 1));
    if(dst[idx] != UINT8_MAX) {
        dst[idx] += 1;
        rv = cldm_avx2_memcmp(dst, data, size);
        if(rv <= 0) {
            fprintf(stderr, "%d: Expected positive value but got %d\n", __LINE__, rv);
            dump(dst, data, size);
            goto epilogue;
        }

        rv = cldm_avx2_memcmp(data, dst, size);
        if(rv >= 0) {
            fprintf(stderr, "%d: Expected negative value but got %d\n", __LINE__, rv);
            dump(dst, data, size);
            goto epilogue;
        }
        dst[idx] -= 1;
    }

    if(dst[idx]) {
        dst[idx] -= 1;
        rv = cldm_avx2_memcmp(dst, data, size);
        if(rv >= 0) {
            fprintf(stderr, "%d: Expected negative value but got %d\n", __LINE__, rv);
            dump(dst, data, size);
            goto epilogue;
        }

        rv = cldm_avx2_memcmp(data, dst, size);
        if(rv <= 0) {
            fprintf(stderr, "%d: Expected positive value but got %d\n", __LINE__, rv);
            dump(dst, data, size);
            goto epilogue;
        }
        dst[idx] += 1;
    }

    crash = false;
epilogue:
    free(dst);
    if(crash) {
        abort();
    }
    return 0;
}
