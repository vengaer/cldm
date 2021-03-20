#ifndef CLDM_TEST_H
#define CLDM_TEST_H

#include "cldm_elf.h"
#include "cldm_macro.h"

#include <stddef.h>

#include <sys/types.h>

#define cldm_test_proc_prefix       \
    cldm_test_proc_
#define cldm_local_setup_ident      \
    cldm_local_setup
#define cldm_local_teardown_ident   \
    cldm_local_teardown
#define cldm_global_setup_ident     \
    cldm_global_setup
#define cldm_global_teardown_ident  \
    cldm_global_teardown

#define CLDM_TEST(name)             \
    void cldm_cat_expand(cldm_test_proc_prefix,name)(void)

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
#endif

ssize_t cldm_test_collect(char *restrict buffer, struct cldm_elfmap const *restrict map, size_t bufsize);
int cldm_test_invoke_each(char const *tests, size_t ntotal);

#endif /* CLDM_TEST_H */
