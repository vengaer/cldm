#include "avx2_strcmp_fuzz.h"
#include "memory_utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int cldm_avx2_strcmp(char const *string0, char const *string1);

static inline bool reseq(int l, int r) {
    return (l < 0 && r < 0) || (!l && !r) || (l > 0 && r > 0);
}

static void report_error(char const *restrict desc, int res0, int res1, char const *str0, char const *str1, size_t size) {
    fprintf(stderr, "Error encountered in %s pass\n", desc);
    fprintf(stderr, "Reported: %d\n", res0);
    fprintf(stderr, "Actual: %d\n", res1);
    fprintf(stderr, "str0: '%s'\n", str0);
    hexdump("str0 hex", (uint8_t const *)str0, size, stderr);
    fprintf(stderr, "  Address: %p\n", (void const *)str0);
    fprintf(stderr, "  Distance to page boundary: %zu\n", pgdistance(str0));
    fprintf(stderr, "str1: '%s'\n", str1);
    hexdump("str1 hex", (uint8_t const *)str1, size, stderr);
    fprintf(stderr, "  Address: %p\n", (void const *)str1);
    fprintf(stderr, "  Distance to page boundary: %zu\n", pgdistance(str1));
}

int avx2_strcmp_fuzz(uint8_t const *data, size_t size) {
    char *str0, *str1;
    int res0, res1;
    bool crash;

    if(!size) {
        return 0;
    }

    str1 = 0;
    str0 = malloc(size + 1);
    if(!str0) {
        fputs("malloc failure\n", stderr);
        return 0;
    }

    crash = true;
    str1 = malloc(size + 1);
    if(!str1) {
        fputs("malloc failure\n", stderr);
        goto epilogue;
    }

    for(unsigned i = 0; i < size; i++) {
        str0[i] = asciicvt(data[i]);
        str1[i] = str0[i];
    }
    str0[size] = '\0';
    str1[size] = '\0';

    for(unsigned i = 0; i < size; i++) {
        res0 = cldm_avx2_strcmp(str0 + i, str1 + i);
        res1 = strcmp(str0 + i, str1 + i);
        if(!reseq(res0, res1)) {
            report_error("eq", res0, res1, str0 + i, str1 + i, size - i);
            goto epilogue;
        }
    }

    for(unsigned i = 0; i < size; i++) {
        str0[i] = asciicvt((uint8_t)(str0[i] - 1));
        for(unsigned j = 0; j < size; j++) {
            res0 = cldm_avx2_strcmp(str0 + j, str1 + j);
            res1 = strcmp(str0 + j, str1 + j);
            if(!reseq(res0, res1)) {
                report_error("lt", res0, res1, str0 + j, str1 + j, size - j);
                goto epilogue;
            }
        }
        str1[i] = str0[i];
    }

    for(unsigned i = 0; i < size; i++) {
        str0[i] = asciicvt((uint8_t)(str0[i] + 1));
        for(unsigned j = 0; j < size; j++) {
            res0 = cldm_avx2_strcmp(str0 + j, str1 + j);
            res1 = strcmp(str0 + j, str1 + j);
            if(!reseq(res0, res1)) {
                report_error("gt", res0, res1, str0 + j, str1 + j, size - j);
                goto epilogue;
            }
        }
        str1[i] = str0[i];
    }

    crash = false;
epilogue:
    free(str0);
    if(str1) {
        free(str1);
    }
    if(crash) {
        abort();
    }
    return 0;
}
