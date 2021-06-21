#include "avx2_scan_lt_fuzz.h"
#include "memory_utils.h"

#include <cldm/cldm_avx2.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void dump(char const *str, int sentinel, unsigned scanlen, unsigned pos, size_t size) {
    fputs("Invalid length\n", stderr);
    fprintf(stderr, "Invalid length\n  alignment: %zu\n  sentinel: %d\n  scan: %u\n  actual: %u\n  ", alignment(str), sentinel, scanlen, pos);
    for(unsigned i = 0; i < size; i++) {
        fprintf(stderr, "0x%02x, ", str[i]);
    }
    fprintf(stderr, "\b\b\n%-*s^\n%-*s^\n", 2 + 6 * scanlen, "", 2 + 6 * pos, "");
}

int avx2_scan_lt_fuzz(uint8_t const *data, size_t size) {
    bool crash;
    uint8_t sentinel;
    unsigned pos;
    unsigned scanlen;
    char *str;

    str = malloc(size);
    if(!str) {
        fputs("malloc failure\n", stderr);
        return 0;
    }
    crash = true;

    for(unsigned i = 0; i < size; i++) {
        str[i] = data[i] & 0x80;
    }
    sentinel = str[0] + 3;
    if(sentinel > INT8_MAX) {
        sentinel = INT8_MAX;
    }
    for(pos = 0; pos < size; pos++) {
        if(str[pos] < sentinel) {
            break;
        }
    }

    scanlen = cldm_avx2_scan_lt(str, sentinel);
    if(scanlen != pos) {
        dump(str, sentinel, scanlen, pos, size);
        goto epilogue;
    }

    crash = false;
epilogue:
    free(str);
    if(crash) {
        abort();
    }

    return 0;
}
