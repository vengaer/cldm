#ifndef CLDM_TEST_H
#define CLDM_TEST_H

#include "cldm_config.h"
#include "cldm_elf.h"
#include "cldm_macro.h"
#include "cldm_rbtree.h"
#include "cldm_testrec.h"

#include <stdbool.h>
#include <stddef.h>

#include <sys/types.h>

ssize_t cldm_test_collect(struct cldm_rbtree *restrict tree, struct cldm_elfmap const *restrict map);
int cldm_test_invoke_each(struct cldm_rbtree const *restrict tests, struct cldm_elfmap const *restrict map, size_t ntotal, bool fail_fast);

#define cldm_testproc_prefix        \
    cldm_testproc_
#define cldm_local_setup_ident      \
    cldm_local_setup
#define cldm_local_teardown_ident   \
    cldm_local_teardown
#define cldm_global_setup_ident     \
    cldm_global_setup
#define cldm_global_teardown_ident  \
    cldm_global_teardown
#define cldm_testrec_prefix         \
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

void cldm_assert_internal(bool eval, char const *restrict expr, char const *restrict file, char const *restrict line);

#define CLDM_ASSERT_TRUE(expr)      \
    cldm_assert_internal(expr, #expr, __FILE__, cldm_str_expand(__LINE__))

#define CLDM_ASSERT_FALSE(expr)     \
    CLDM_ASSERT_TRUE(!(expr))

#ifdef CLDM_HAS_GENERIC

#define cldm_genassert_typelist \
    c,   int,                   \
    llu, unsigned long long,    \
    lld, long long,             \
    p,   void const *

#define cldm_genassert_decls3(opstr, suffix, type)          \
    void cldm_assert_ ## opstr ## _ ## suffix(type l, type r, char const *restrict lexpand, char const *restrict rexpand, char const *restrict file, char const *restrict line);

#define cldm_genassert_decls5(opstr, suffix, type, ...)     \
    cldm_genassert_decls3(opstr, suffix, type)              \
    cldm_genassert_decls3(opstr, __VA_ARGS__)

#define cldm_genassert_decls7(opstr, suffix, type, ...)     \
    cldm_genassert_decls3(opstr, suffix, type)              \
    cldm_genassert_decls5(opstr, __VA_ARGS__)

#define cldm_genassert_decls9(opstr, suffix, type, ...)     \
    cldm_genassert_decls3(opstr, suffix, type)              \
    cldm_genassert_decls7(opstr, __VA_ARGS__)

#define cldm_genassert_decls(...) \
    cldm_cat_expand(cldm_genassert_decls, cldm_count(__VA_ARGS__))(__VA_ARGS__)

cldm_genassert_decls(eq, cldm_genassert_typelist)
cldm_genassert_decls(ne, cldm_genassert_typelist)
cldm_genassert_decls(gt, cldm_genassert_typelist)
cldm_genassert_decls(ge, cldm_genassert_typelist)
cldm_genassert_decls(lt, cldm_genassert_typelist)
cldm_genassert_decls(le, cldm_genassert_typelist)

#define cldm_assert_generic(l, r, op)                       \
    _Generic((l),                                           \
        bool:               cldm_assert_ ## op ## _lld,     \
        char:               cldm_assert_ ## op ## _c,       \
        unsigned char:      cldm_assert_ ## op ## _llu,     \
        unsigned short:     cldm_assert_ ## op ## _llu,     \
        unsigned:           cldm_assert_ ## op ## _llu,     \
        unsigned long:      cldm_assert_ ## op ## _llu,     \
        unsigned long long: cldm_assert_ ## op ## _llu,     \
        signed char:        cldm_assert_ ## op ## _lld,     \
        short:              cldm_assert_ ## op ## _lld,     \
        int:                cldm_assert_ ## op ## _lld,     \
        long:               cldm_assert_ ## op ## _lld,     \
        long long:          cldm_assert_ ## op ## _lld,     \
        default:            cldm_assert_ ## op ## _p        \
    )(l, r, #l, #r, __FILE__, cldm_str_expand(__LINE__))

#define CLDM_ASSERT_EQ(l, r)    \
    cldm_assert_generic(l, r, eq)

#define CLDM_ASSERT_NE(l, r)    \
    cldm_assert_generic(l, r, ne)

#define CLDM_ASSERT_GT(l, r)    \
    cldm_assert_generic(l, r, gt)

#define CLDM_ASSERT_GE(l, r)    \
    cldm_assert_generic(l, r, ge)

#define CLDM_ASSERT_LT(l, r)    \
    cldm_assert_generic(l, r, lt)

#define CLDM_ASSERT_LE(l, r)    \
    cldm_assert_generic(l, r, le)

#else /* CLDM_HAS_GENERIC */

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

#endif /* CLDM_HAS_GENERIC */

#endif /* CLDM_TEST_H */
