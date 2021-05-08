#include "argp_fuzz.h"

#include <cldm/cldm_argp.h>

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int argp_fuzz(uint8_t const *data, size_t size) {
    char **strings;
    char *buffer;
    unsigned nstrings;
    bool is_null;

    if(!size) {
        return 0;
    }

    buffer = 0;
    strings = malloc(size * sizeof(*strings));

    if(!strings) {
        fputs("malloc failure\n", stderr);
        goto epilogue;
    }

    buffer = malloc(size);
    if(!buffer) {
        fputs("malloc failure\n", stderr);
        goto epilogue;
    }

    memcpy(buffer, data, size);
    buffer[size - 1] = 0;

    nstrings = 0;
    is_null = false;
    for(unsigned i = 0; i < size; i++) {
        if(!isprint(buffer[i])) {
            is_null = true;
            buffer[i] = 0;
        }
        else if(is_null) {
            is_null = false;
            strings[nstrings++] = &buffer[i];
        }
    }

    nstrings %= CLDM_ARGP_MAX_PARAMS + 2;

    cldm_argp_parse(&(struct cldm_args) { 0 }, (int)nstrings, strings);

epilogue:
    if(strings) {
        free(strings);
    }
    if(buffer) {
        free(buffer);
    }
    return 0;
}
