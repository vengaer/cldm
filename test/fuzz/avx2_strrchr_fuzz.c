#include "avx2_strrchr_fuzz.h"
#include "memory_utils.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char *cldm_avx2_strrchr(char const *str, int c);

static void report_error(char const *res0, char const *res1, char const *str, int c, size_t size) {
    fprintf(stderr, "Error encountered looking for %c (%d)\n", (char)c, c);
    fprintf(stderr, "String: '%s'\n", str);
    fprintf(stderr, "  Size: %zu\n", size);
    fprintf(stderr, "  Address: %p\n", (void const *)str);
    fprintf(stderr, "  Alignment :%zu\n", alignment(str));
    fprintf(stderr, "  Distance to page boundary: %zu\n", pgdistance(str));
    fprintf(stderr, "Reported: '%s'\n", res0);
    fprintf(stderr, "  Offset %zu\n", (size_t)(res0 - str));
    fprintf(stderr, "Actual: '%s'\n", res1);
    fprintf(stderr, "  Offset %zu\n", (size_t)(res1 - str));
}

int avx2_strrchr_fuzz(uint8_t const *data, size_t size) {
    char *str;
    char *res0, *res1;
    bool crash;

    if(!size) {
        return 0;
    }

    str = malloc(size + 1);
    if(!str) {
        fputs("malloc failure\n", stderr);
        return 0;
    }

    crash = true;

    str[size] = '\0';
    for(unsigned i = 0; i < size; i++) {
        str[i] = asciicvt(data[i]);
    }

    for(unsigned i = 0; i < size; i++) {
        for(unsigned j = 0; j < CHAR_MAX; j++) {
            res0 = cldm_avx2_strrchr(str + i, (char)j);
            res1 = strrchr(str + i, (char)j);
            if(res0 != res1) {
                report_error(res0, res1, str + i, (char)j, size - i);
                goto epilogue;
            }
        }
    }

    crash = false;
epilogue:
    free(str);
    if(crash) {
        abort();
    }
    return 0;
}
