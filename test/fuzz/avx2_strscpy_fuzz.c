#include "avx2_strscpy_fuzz.h"
#include "memory_utils.h"

#include <cldm/cldm_avx2.h>

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { VECSIZE = 32 };
enum { STRINGSIZE = 8192 };

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
    fprintf(stderr, "  length %zu\n", strlen(dst));
    fprintf(stderr, "  address: %p\n", (void const *)dst);
    fprintf(stderr, "  alignment: %zu\n", alignment(dst));
    fprintf(stderr, "  distance to page boundary: %zu\n", pgdistance(dst));
}

int avx2_strscpy_fuzz(uint8_t const *data, size_t size) {
    size_t dstsize;
    long long res;
    bool crash;
    char *src;
    char *dst;

    crash = true;

    if(size < VECSIZE + 1) {
        return 0;
    }

    src = malloc(STRINGSIZE);
    if(!src) {
        return 0;
    }

    dst = malloc(STRINGSIZE);
    if(!dst) {
        goto epilogue;
    }
    size = size > STRINGSIZE ? STRINGSIZE : size;

    dstsize = ((double)data[0] / UCHAR_MAX) * STRINGSIZE;
    for(unsigned i = 0; i < size; i++) {
        src[i] = asciicvt(data[i]);
    }
    src[size - 1] = 0;

    for(unsigned i = 0; i < size; i++) {
        res = cldm_avx2_strscpy(dst, &src[i], dstsize);
        if(res == -7) {
            if(size - i < dstsize) {
                report_error("erroneous E2BIG", &src[i], dst, size - i, dstsize);
                goto epilogue;
            }
            else if(dstsize && dst[dstsize - 1]) {
                report_error("missing null terminator on E2BIG", &src[i], dst, size - i, dstsize);
                goto epilogue;
            }
        }
        else if(res != (long long)strlen(&src[i])) {
            report_error("incorrect return value", &src[i], dst, size - i, dstsize);
            goto epilogue;
        }
        else if(strncmp(dst, &src[i], res)) {
            report_error("incorrect string written", &src[i], dst, size - i, dstsize);
            goto epilogue;
        }
        else if(dst[res]) {
            report_error("missing null terminator", &src[i], dst, size - i, dstsize);
            goto epilogue;
        }
    }

    crash = false;

epilogue:
    if(src) {
        free(src);
    }
    if(dst) {
        free(dst);
    }

    if(crash) {
        abort();
    }

    return 0;
}
