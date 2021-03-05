#ifndef CLDM_NTBS_H
#define CLDM_NTBS_H

#include "cldm_limits.h"
#include "cldm_macro.h"

#include <errno.h>
#include <stddef.h>
/* TODO: remove */
#include <string.h>

#include <sys/types.h>

#define cldm_for_each_word(iter, str)                                                   \
    for(char cldm_cat_expand(cldm_few_buf,__LINE__)[CLDM_PATH_MAX],                     \
            *cldm_cat_expand(cldm_few_end,__LINE__) =                                   \
                (iter = strcpy(cldm_cat_expand(cldm_few_buf,__LINE__), str),            \
                    strchr(iter, ' '));                                           \
        (cldm_cat_expand(cldm_few_end,__LINE__) ?                                       \
            *cldm_cat_expand(cldm_few_end,__LINE__) = '\0' :                            \
            0, iter);                                                                   \
        iter = cldm_cat_expand(cldm_few_end,__LINE__) ?                                 \
            cldm_cat_expand(cldm_few_end,__LINE__) + 1 : 0,                             \
        cldm_cat_expand(cldm_few_end,__LINE__) ?                                        \
            cldm_cat_expand(cldm_few_end,__LINE__) =                                    \
                strchr(cldm_cat_expand(cldm_few_end,__LINE__) + 1, ' ') : 0)

char *cldm_ntbschr(char *str, int c);

ssize_t cldm_ntbscpy(char *restrict dst, char const *restrict src, size_t dstsize);

#endif /* CLDM_NTBS_H */
