#ifndef CLDM_TEST_H
#define CLDM_TEST_H

#include "cldm_auxprocs.h"
#include "cldm_collect.h"
#include "cldm_config.h"
#include "cldm_macro.h"
#include "cldm_testrec.h"
#include "cldm_type.h"

#include <stdbool.h>
#include <stddef.h>

/* Must be called in a sequential context */
void cldm_test_register(size_t ntests, bool verbose);
int cldm_test_summary(void);

/* Thread safe */
bool cldm_test_init(unsigned thread_id);
void cldm_test_free(unsigned thread_id);
bool cldm_test_run(unsigned thread_id, struct cldm_testrec const *restrict record, struct cldm_auxprocs const *restrict auxprocs, bool fail_fast);
void cldm_test_flush(unsigned thread_id);

void cldm_test_reduce_mark_ready(unsigned thread_id);
void cldm_test_reduce(unsigned thread_id, unsigned offset);

void cldm_assert_true(bool eval, char const *restrict expr, char const *restrict file, char const *restrict line);
void cldm_assert_streq(char const *restrict l, char const *restrict r, long long n, char const *restrict lname, char const *restrict rname, char const *restrict file, char const *restrict line);

#define cldm_testproc_prefix        \
    cldm_testproc_
#define cldm_testrec_prefix         \
    cldm_testrec_

#define cldm_gen_test(testname, ...)                                      \
    void cldm_cat_expand(cldm_testproc_prefix,testname)(void);              \
    struct cldm_testrec cldm_cat_expand(cldm_testrec_prefix,testname) = {   \
        .name = #testname,                                                  \
        .file = __FILE__,                                                   \
        .handle = cldm_cat_expand(cldm_testproc_prefix,testname),           \
        .runinfo = __VA_ARGS__                                              \
    };                                                                      \
    void cldm_cat_expand(cldm_testproc_prefix,testname)(void)

#define cldm_test3(...)             \
    cldm_gen_test(__VA_ARGS__)
#define cldm_test2(...)             \
    cldm_gen_test(__VA_ARGS__)
#define cldm_test1(testname)        \
    cldm_gen_test(testname, { 0 })

#define CLDM_TEST(...)              \
    cldm_overload(cldm_test, __VA_ARGS__)

#ifndef CLDM_PREFIX_ONLY
#define TEST(...)             CLDM_TEST(__VA_ARGS__)

#define ASSERT_TRUE(...)      CLDM_ASSERT_TRUE(__VA_ARGS__)
#define ASSERT_FALSE(...)     CLDM_ASSERT_FALSE(__VA_ARGS__)
#define ASSERT_EQ(...)        CLDM_ASSERT_EQ(__VA_ARGS__)
#define ASSERT_NE(...)        CLDM_ASSERT_NE(__VA_ARGS__)
#define ASSERT_LT(...)        CLDM_ASSERT_LT(__VA_ARGS__)
#define ASSERT_LE(...)        CLDM_ASSERT_LE(__VA_ARGS__)
#define ASSERT_GT(...)        CLDM_ASSERT_GT(__VA_ARGS__)
#define ASSERT_GE(...)        CLDM_ASSERT_GE(__VA_ARGS__)
#define ASSERT_STREQ(...)     CLDM_ASSERT_STREQ(__VA_ARGS__)
#define ASSERT_STRNEQ(...)    CLDM_ASSERT_STRNEQ(__VA_ARGS__)
#endif

#define CLDM_ASSERT_TRUE(expr)      \
    cldm_assert_true(expr, #expr, __FILE__, cldm_str_expand(__LINE__))

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

#define CLDM_ASSERT_STREQ(l, r)     \
    cldm_assert_streq(l, r, -1, #l, #r, __FILE__, cldm_str_expand(__LINE__))

#define CLDM_ASSERT_STRNEQ(l, r, n) \
    cldm_assert_streq(l, r, n, #l, #r, __FILE__, cldm_str_expand(__LINE__))


#endif /* CLDM_TEST_H */
