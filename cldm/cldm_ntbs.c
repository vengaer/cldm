#include "cldm_ntbs.h"

char *cldm_ntbschr(char *str, int c) {
    while(*str != (char)c && *str) {
        ++str;
    }
    return *str ? str : 0;
}

char const *cldm_ntbscrchr(char const *str, int c) {
    char const *res = 0;
    while(*str++) {
        if(*str == c) {
            res = str;
        }
    }
    return res;
}

size_t cldm_ntbslen(char const *str) {
    char const *s = str;
    while(*s++);

    return s - str - 1;
}

ssize_t cldm_ntbscpy(char *restrict dst, char const *restrict src, size_t dstsize) {
    size_t const srclen = cldm_ntbslen(src);
    size_t i = dstsize;
    char *d = dst;

    while(i-- && (*d++ = *src++));

    if(srclen >= dstsize) {
        dst[dstsize - 1] = '\0';
        return -E2BIG;
    }

    return srclen;
}

char const *cldm_ntbs_find_substr(char const *restrict str, char const *restrict substr) {
    char const *d = substr;

    for(;*str && *d; ++str) {
        d = *str == *d ? d + 1 : substr;
    }

    return *d ? 0 : str - cldm_ntbslen(substr);
}

int cldm_ntbscmp(char const *restrict str0, char const *restrict str1) {
    int diff = 0;
    do {
        diff = -1 * (*str0 < *str1) + (*str0 > *str1);
    } while(*str0++ && *str1++ && !diff);

    return diff;
}

int cldm_ntbsncmp(char const *restrict str0, char const *restrict str1, size_t n) {
    int diff = 0;
    do {
        diff = -1 * (*str0 < *str1) + (*str0 > *str1);
    } while(*str0++ && *str1++ && !diff && --n);
    return diff;
}
