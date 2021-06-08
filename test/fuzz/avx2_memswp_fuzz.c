#include "avx2_memswp_fuzz.h"
#include "memory_utils.h"

#include <cldm/cldm_config.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void cldm_avx2_memswp(void *restrict s0, void *restrict s1, unsigned long long n);

void report(unsigned idx, uint8_t const *restrict bs, uint8_t const *restrict ref, size_t size) {
    fprintf(stderr, "bs%u differs\n", idx);
    fputs("bs: ", stderr);
    for(unsigned i = 0; i < size; i++) {
        fprintf(stderr, "%02x", bs[i]);
    }
    fputs("\nref: ", stderr);
    for(unsigned i = 0; i < size; i++) {
        fprintf(stderr, "%02x", ref[i]);
    }
    fprintf(stderr, "\nLength: %zu\n", size);
    fprintf(stderr, "Address: %p\n", (void const *)bs);
    fprintf(stderr, "Alignment: %zu\n", alignment(bs));
}

int avx2_memswp_fuzz(uint8_t const *data, size_t size) {
    bool crash;
    uint8_t *bs0;
    uint8_t *bs1;
    uint8_t *ref0;
    if(!size) {
        return 0;
    }

    crash = false;
    bs0 = malloc(3 * size * sizeof(uint8_t));
    if(!bs0) {
        fputs("malloc failure\n", stderr);
        return 0;
    }
    bs1 = bs0 + size * sizeof(uint8_t);
    ref0 = bs1 + size * sizeof(uint8_t);

    memcpy(bs0, data, size);
    memcpy(bs1, data, size);

    for(unsigned i = 0; i < size; i++) {
        bs1[i] ^= 1;
    }

    memcpy(ref0, bs1, size);

    cldm_avx2_memswp(bs0, bs1, size);

    if(memcmp(bs0, ref0, size)) {
        report(0, bs0, ref0, size);
        crash = true;
    }
    if(memcmp(bs1, data, size)) {
        report(1, bs1, data, size);
        crash = true;
    }

    if(bs0) {
        free(bs0);
    }
    if(crash) {
        abort();
    }
    return 0;
}
