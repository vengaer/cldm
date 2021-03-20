#ifndef CLDM_NTBS_H
#define CLDM_NTBS_H

#include "cldm_limits.h"
#include "cldm_macro.h"

#include <errno.h>
#include <stddef.h>

#include <sys/types.h>

/* TODO assert ntbscpy result */
/* TODO ntbscpy tests */
#define cldm_for_each_word3(iter, str, sep)                                                 \
    for(char cldm_cat_expand(cldm_few_buf,__LINE__)[CLDM_PATH_MAX],                         \
            *cldm_cat_expand(cldm_few_end,__LINE__) =                                       \
                (cldm_ntbscpy(cldm_cat_expand(cldm_few_buf,__LINE__), str,                  \
                    sizeof(cldm_cat_expand(cldm_few_buf,__LINE__))),                        \
                iter = cldm_cat_expand(cldm_few_buf,__LINE__),                              \
                cldm_ntbschr(iter, sep));                                                   \
        (cldm_cat_expand(cldm_few_end,__LINE__) ?                                           \
            *cldm_cat_expand(cldm_few_end,__LINE__) = '\0' :                                \
            0, *iter);                                                                      \
        iter = cldm_cat_expand(cldm_few_end,__LINE__) ?                                     \
            cldm_cat_expand(cldm_few_end,__LINE__) + 1 : 0,                                 \
        cldm_cat_expand(cldm_few_end,__LINE__) ?                                            \
            cldm_cat_expand(cldm_few_end,__LINE__) =                                        \
                cldm_ntbschr(cldm_cat_expand(cldm_few_end,__LINE__) + 1, sep) : 0)

#define cldm_for_each_word2(iter, str)  \
    cldm_for_each_word3(iter, str, ' ')

#define cldm_for_each_word(...) \
    cldm_overload(cldm_for_each_word,__VA_ARGS__)

char *cldm_ntbschr(char *str, int c);
char const *cldm_ntbscrchr(char const *str, int c);
size_t cldm_ntbslen(char const *str);
ssize_t cldm_ntbscpy(char *restrict dst, char const *restrict src, size_t dstsize);

char const *cldm_ntbs_find_substr(char const *restrict str, char const *restrict substr);

int cldm_ntbscmp(char const *restrict str0, char const *restrict str1);
int cldm_ntbsncmp(char const *restrict str0, char const *restrict str1, size_t n);

#endif /* CLDM_NTBS_H */
