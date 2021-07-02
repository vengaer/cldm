#include "avx2_strscpy_fuzz.h"
#include "memory_utils.h"

#include <cldm/cldm_avx2.h>
#include <cldm/cldm_config.h>

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { VECSIZE = 32 };
enum { BUFSIZE = 2 * CLDM_PGSIZE };
enum { STRSIZE = 8 * VECSIZE + 2 };

static void report_error(char const *restrict type, char const *restrict src, char const *restrict dst, size_t srcsize, size_t dstsize) {
    fprintf(stderr, "Error encountered: %s\n", type);
    fprintf(stderr, "src: '%s'\n", src);
    fprintf(stderr, "  size: %zu\n", srcsize);
    fprintf(stderr, "  length: %zu\n", strlen(src));
    fprintf(stderr, "  address: %p\n", (void const *)src);
    fprintf(stderr, "  alignment: %zu\n", alignment(src));
    fprintf(stderr, "  distance to page boundary: %zu\n", pgdistance(src));
    fprintf(stderr, "dst: '%s'\n", dst);
    fprintf(stderr, "  size: %zu\n", dstsize);
    fprintf(stderr, "  length: %zu\n", strlen(dst));
    fprintf(stderr, "  address: %p\n", (void const *)dst);
    fprintf(stderr, "  alignment: %zu\n", alignment(dst));
    fprintf(stderr, "  distance to page boundary: %zu\n", pgdistance(dst));
}

int avx2_strscpy_fuzz(uint8_t const *data, size_t size) {
    long long res;
    bool crash;
    char *src;
    char *srcbase;
    char *dst;
    char *dstbase;
    size_t dstsize;

    crash = true;
    /* Closer 256 bytes to pgboundary */
    /* Closer 256 bytes to bufend */
    /* Strings shorter than 32 bytes */

    if(!size) {
        return 0;
    }

    srcbase = malloc(BUFSIZE);
    if(!srcbase) {
        fputs("malloc failure\n", stderr);
        return 0;
    }


    dstbase = malloc(BUFSIZE);
    if(!dstbase) {
        fputs("malloc failure\n", stderr);
        goto epilogue;
    }

    src = (char *)((((uintptr_t)srcbase + CLDM_PGSIZE) & ~(CLDM_PGSIZE - 1)) - STRSIZE / 2);
    dst = (char *)((((uintptr_t)dstbase + CLDM_PGSIZE) & ~(CLDM_PGSIZE - 1)) - STRSIZE / 2);

    size = size > STRSIZE ? STRSIZE : size;
    dstsize = size;

    for(unsigned i = 0; i < size; i++) {
        src[i] = asciicvt(data[i]);
    }
    src[size - 1] = 0;

    for(unsigned i = 0; i < size; i++) {
        for(unsigned j = 0; j <= dstsize; j++) {
            res = cldm_avx2_strscpy(dst, &src[i], dstsize - j);
            if(res == -7) {
                if(size - i < dstsize - j) {
                    report_error("erroneous E2BIG", &src[i], dst, size - i, dstsize - j);
                    goto epilogue;
                }
                else if(dstsize - j && dst[dstsize - j - 1]) {
                    report_error("missing null terminator on E2BIG", &src[i], dst, size - i, dstsize - j);
                    goto epilogue;
                }
            }
            else if(res != (long long)strlen(&src[i])) {
                report_error("incorrect return value", &src[i], dst, size - i, dstsize - j);
                goto epilogue;
            }
            else if(strncmp(dst, &src[i], res)) {
                report_error("incorrect string written", &src[i], dst, size - i, dstsize - j);
                goto epilogue;
            }
            else if(dst[res]) {
                report_error("missing null terminator", &src[i], dst, size - i, dstsize - j);
                goto epilogue;
            }
        }
    }

    crash = false;

epilogue:
    free(srcbase);
    if(dstbase) {
        free(dstbase);
    }

    if(crash) {
        abort();
    }

    return 0;
}
