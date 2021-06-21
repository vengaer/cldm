#include "avx2_strlen_fuzz.h"
#include "memory_utils.h"

#include <cldm/cldm_avx2.h>

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void report_error(char const *str, unsigned long long reported, size_t actual) {
    fputs("Error encountered\n", stderr);
    fprintf(stderr, "String: '%s'\n", str);
    fprintf(stderr, "Reported length: %llu\n", reported);
    fprintf(stderr, "Actual length: %zu\n", actual);
    fprintf(stderr, "Address: %p\n", (void const *)str);
    fprintf(stderr, "Alignment: %zu\n", alignment(str));
    fprintf(stderr, "Distance to page boundary: %zu\n", pgdistance((void const *)str));
}

int avx2_strlen_fuzz(uint8_t const *data, size_t size) {
    char *str;
    bool crash;
    unsigned long long reported;
    size_t actual;

    if(!size) {
        return 0;
    }

    str = malloc(size);
    if(!str) {
        fputs("malloc failure\n", stderr);
        return 0;
    }
    for(unsigned i = 0; i < size - 1; i++) {
        str[i] = asciicvt(data[i]);
    }
    str[size - 1] = 0;

    crash = true;
    actual = strlen(str);

    for(unsigned i = 0; i < actual; i++) {
        reported = cldm_avx2_strlen(str + i);
        if(reported != actual - i) {
            report_error(str + i, reported, actual - i);
            goto epilogue;
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
