#ifndef CLDM_TEST_H
#define CLDM_TEST_H

#include "cldm_elf.h"
#include "cldm_macro.h"
#include "cldm_rbtree.h"
#include "cldm_testrec.h"

#include <stdbool.h>
#include <stddef.h>

#include <sys/types.h>

ssize_t cldm_test_collect(cldm_rbtree *restrict tree, struct cldm_elfmap const *restrict map);
int cldm_test_invoke_each(cldm_rbtree const *restrict tests, struct cldm_elfmap const *restrict map, size_t ntotal);

void cldm_test_assertion(char const *restrict expr, char const *restrict file, char const *restrict line, bool result);

#define cldm_testproc_prefix       \
    cldm_testproc_
#define cldm_local_setup_ident      \
    cldm_local_setup
#define cldm_local_teardown_ident   \
    cldm_local_teardown
#define cldm_global_setup_ident     \
    cldm_global_setup
#define cldm_global_teardown_ident  \
    cldm_global_teardown

#define cldm_testrec_prefix        \
    cldm_testrec_

#define CLDM_TEST(testname)                                                 \
    void cldm_cat_expand(cldm_testproc_prefix,testname)(void);              \
    struct cldm_testrec cldm_cat_expand(cldm_testrec_prefix,testname) = {   \
        .name = #testname,                                                  \
        .file = __FILE__,                                                   \
        .handle = cldm_cat_expand(cldm_testproc_prefix,testname)            \
    };                                                                      \
    void cldm_cat_expand(cldm_testproc_prefix,testname)(void)

#define CLDM_TEST_SETUP()           \
    void cldm_expand(cldm_local_setup_ident)(void)

#define CLDM_TEST_TEARDOWN()        \
    void cldm_expand(cldm_local_teardown_ident)(void)

#define CLDM_GLOBAL_SETUP()         \
    void cldm_expand(cldm_global_setup_ident)(void)

#define CLDM_GLOBAL_TEARDOWN()      \
    void cldm_expand(cldm_global_teardown_ident)(void)

#define CLDM_ASSERT_TRUE(expr)      \
    cldm_test_assertion(#expr, __FILE__, cldm_str_expand(__LINE__), expr)

#define CLDM_ASSERT_FALSE(expr)     \
    CLDM_ASSERT_TRUE(!(expr))

#define CLDM_ASSERT_EQ(l, r)        \
    CLDM_ASSERT_TRUE((l) == (r))

#define CLDM_ASSERT_NE(l, r)        \
    CLDM_ASSERT_TRUE((l) != (r))

#define CLDM_ASSERT_LT(l, r)        \
    CLDM_ASSERT_TRUE((l) < (r))

#define CLDM_ASSERT_LE(l, r)        \
    CLDM_ASSERT_TRUE((l) <= (r))

#define CLDM_ASSERT_GT(l, r)        \
    CLDM_ASSERT_TRUE((l) > (r))

#define CLDM_ASSERT_GE(l, r)        \
    CLDM_ASSERT_TRUE((l) >= (r))

#ifndef CLDM_PREFIX_ONLY
#define TEST(...)         CLDM_TEST(__VA_ARGS__)

#define TEST_SETUP()      CLDM_TEST_SETUP()
#define TEST_TEARDOWN()   CLDM_TEST_TEARDOWN()

#define GLOBAL_SETUP()    CLDM_GLOBAL_SETUP()
#define GLOBAL_TEARDOWN() CLDM_GLOBAL_TEARDOWN()

#define ASSERT_TRUE(...)  CLDM_ASSERT_TRUE(__VA_ARGS__)
#define ASSERT_FALSE(...) CLDM_ASSERT_FALSE(__VA_ARGS__)
#define ASSERT_EQ(...)    CLDM_ASSERT_EQ(__VA_ARGS__)
#define ASSERT_NE(...)    CLDM_ASSERT_NE(__VA_ARGS__)
#define ASSERT_LT(...)    CLDM_ASSERT_LT(__VA_ARGS__)
#define ASSERT_LE(...)    CLDM_ASSERT_LE(__VA_ARGS__)
#define ASSERT_GT(...)    CLDM_ASSERT_GT(__VA_ARGS__)
#define ASSERT_GE(...)    CLDM_ASSERT_GE(__VA_ARGS__)
#endif

#endif /* CLDM_TEST_H */
