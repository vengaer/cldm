#ifndef CLDM_NTBS_H
#define CLDM_NTBS_H

#include "cldm_limits.h"
#include "cldm_macro.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <sys/types.h>

#define cldm_for_each_word3(iter, str, sep)                                                 \
    for(char cldm_cat_expand(cldm_few_buf,__LINE__)[CLDM_PATH_MAX],                         \
            *cldm_cat_expand(cldm_few_end,__LINE__) =                                       \
                (cldm_strscpy(cldm_cat_expand(cldm_few_buf,__LINE__), str,                  \
                    sizeof(cldm_cat_expand(cldm_few_buf,__LINE__))),                        \
                iter = cldm_cat_expand(cldm_few_buf,__LINE__),                              \
                strchr(iter, sep));                                                         \
        (cldm_cat_expand(cldm_few_end,__LINE__) ?                                           \
            *cldm_cat_expand(cldm_few_end,__LINE__) = '\0' :                                \
            0, iter ? *iter : 0);                                                           \
        iter = cldm_cat_expand(cldm_few_end,__LINE__) ?                                     \
            cldm_cat_expand(cldm_few_end,__LINE__) + 1 : 0,                                 \
        cldm_cat_expand(cldm_few_end,__LINE__) ?                                            \
            cldm_cat_expand(cldm_few_end,__LINE__) =                                        \
                strchr(cldm_cat_expand(cldm_few_end,__LINE__) + 1, sep) : 0)

#define cldm_for_each_word2(iter, str)  \
    cldm_for_each_word3(iter, str, ' ')

#define cldm_for_each_word(...) \
    cldm_overload(cldm_for_each_word,__VA_ARGS__)

ssize_t cldm_strscpy(char *restrict dst, char const *restrict src, size_t dstsize);

char const *cldm_ntbs_find_substr(char const *restrict str, char const *restrict substr);

int cldm_ntbscmp(char const *restrict str0, char const *restrict str1);
int cldm_ntbsncmp(char const *restrict str0, char const *restrict str1, size_t n);

#endif /* CLDM_NTBS_H */
