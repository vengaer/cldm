#ifndef CLDM_TEST_H
#define CLDM_TEST_H

#include "cldm_macro.h"

#include <stddef.h>

#include <sys/types.h>

#define cldm_test_proc_prefix cldm_test_proc_

#define CLDM_TEST(name) \
    void cldm_cat_expand(cldm_test_proc_prefix,name)(void)

#ifndef CLDM_PREFIX_ONLY
#define TEST(...) CLDM_TEST(__VA_ARGS__)
#endif

ssize_t cldm_test_collect(char *restrict buffer, char const *restrict file, size_t bufsize);
int cldm_test_invoke_each(char const *tests, size_t ntotal);

#endif /* CLDM_TEST_H */
