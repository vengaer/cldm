#include "cldm_io.h"
#include "cldm_log.h"
#include "cldm_mock.h"
#include "cldm_ntbs.h"
#include "cldm_rtassert.h"
#include "cldm_test.h"

#include <stdlib.h>
#include <string.h>

enum { CLDM_ASSERTION_LEN = 256 };
enum { CLDM_LOG_INITIAL_CAP = 32 };
enum { CLDM_RUNWIDTH = 40 };
enum { CLDM_IDXWIDTH = 10 };

struct cldm_test_log {
    union {
        char (*data)[CLDM_ASSERTION_LEN];
        void *addr;
    } l_un;
    size_t size;
    size_t capacity;
    unsigned long long failed_assertions;
    unsigned long long failed_tests;
    unsigned long long total_assertions;
};

struct cldm_test {
    char const *name;
    bool passed;
};

static struct cldm_test_log cldm_test_log;
static struct cldm_test cldm_current_test;

static void cldm_test_empty_func(void) { }

#define cldm_load_stage(map, stage, scope)                                                  \
    stage = cldm_elf_func(map, cldm_str_expand(cldm_ ## scope ## _ ## stage ## _ident));    \
    if(!stage) {                                                                            \
        stage = cldm_test_empty_func;                                                       \
    }                                                                                       \
    else {                                                                                  \
        cldm_log("Detected " cldm_str_expand(scope) " " cldm_str_expand(stage));            \
    }


static void (*cldm_test_local_setup(struct cldm_elfmap const *map))(void) {
    void (*setup)(void) = { 0 };
    cldm_load_stage(map, setup, local);
    return setup;
}

static void (*cldm_test_local_teardown(struct cldm_elfmap const *map))(void) {
    void (*teardown)(void) = { 0 };
    cldm_load_stage(map, teardown, local);
    return teardown;
}

static void (*cldm_test_global_setup(struct cldm_elfmap const *map))(void) {
    void (*setup)(void) = { 0 };
    cldm_load_stage(map, setup, global);
    return setup;
}

static void (*cldm_test_global_teardown(struct cldm_elfmap const *map))(void) {
    void (*teardown)(void) = { 0 };
    cldm_load_stage(map, teardown, global);
    return teardown;
}

static inline void cldm_test_set(char const *name) {
    cldm_current_test = (struct cldm_test) {
        .name = name,
        .passed = true
    };
}

static int cldm_test_init(void) {
    cldm_test_log.capacity = CLDM_LOG_INITIAL_CAP;
    cldm_test_log.l_un.addr = realloc(0, CLDM_LOG_INITIAL_CAP * sizeof(*cldm_test_log.l_un.data));
    if(!cldm_test_log.l_un.data) {
        cldm_err("Could not allocate chunk of %d bytes", CLDM_LOG_INITIAL_CAP);
        return -1;
    }
    return 0;
}

static inline void cldm_test_free(void) {
    free(cldm_test_log.l_un.addr);
}

static bool cldm_test_ensure_logcapacity(void) {
    void *addr;
    size_t new_cap;

    if(cldm_test_log.size  + 1 >= cldm_test_log.capacity) {
        new_cap = 2 * cldm_test_log.capacity;
        addr = realloc(cldm_test_log.l_un.addr, new_cap * sizeof(*cldm_test_log.l_un.data));
        if(!addr) {
            free(cldm_test_log.l_un.addr);
            return false;
        }
        cldm_test_log.l_un.addr = addr;
        cldm_test_log.capacity = new_cap;
    }

    return true;
}

static void cldm_test_summary(size_t ntests) {
    if(cldm_test_log.failed_tests) {
        cldm_log_stream(cldm_stderr, "\n%llu/%llu assertions failed across %llu test%s",
                        cldm_test_log.failed_assertions, cldm_test_log.total_assertions, cldm_test_log.failed_tests, cldm_test_log.failed_tests == 1 ? "" : "s");
        for(unsigned i = 0; i < cldm_test_log.size; i++) {
            cldm_log_stream(cldm_stderr, "\n%s", cldm_test_log.l_un.data[i]);
        }
        return;
    }
    cldm_log("\nSuccessfully finished %llu assertions across %zu test%s", cldm_test_log.total_assertions, ntests, ntests == 1 ? "" : "s");
}

static int cldm_test_prologue(struct cldm_elfmap const *restrict map, size_t ntests, void(**lcl_setup)(void), void(**lcl_teardown)(void)) {
    void (*glob_setup)(void);

    cldm_log("Collected %zu tests", ntests);

    if(cldm_test_init()) {
        return -1;
    }

    *lcl_setup = cldm_test_local_setup(map);
    *lcl_teardown= cldm_test_local_teardown(map);
    glob_setup = cldm_test_global_setup(map);
    cldm_log("");

    cldm_mock_enable() {
        glob_setup();
    }

    return 0;
}

static int cldm_test_epilogue(struct cldm_elfmap const *restrict map, size_t ntests) {
    void (*glob_teardown)(void);

    glob_teardown = cldm_test_global_teardown(map);

    cldm_mock_enable() {
        glob_teardown();
    }

    cldm_test_summary(ntests);
    cldm_test_free();

    return !!cldm_test_log.failed_tests;
}

static void cldm_test_invoke(struct cldm_testrec const *restrict record, size_t *restrict testidx, size_t ntests, void(*lcl_setup)(void), void(*lcl_teardown)(void)) {
    unsigned runpad;
    unsigned idxpad;
    unsigned length;

    cldm_test_set(record->name);

    length = strlen(record->name) + sizeof("[Running ]") - 1;
    runpad = (length <= CLDM_RUNWIDTH) * (CLDM_RUNWIDTH - length);
    length = snprintf(0, 0, "(%zu/%zu)", ++(*testidx), ntests);
    idxpad = (length <= CLDM_IDXWIDTH) * (CLDM_IDXWIDTH - length);

    cldm_log_raw("[Running %s]%-*s (%zu/%zu)%-*s", record->name, runpad, "", *testidx, ntests, idxpad, "");
    cldm_mock_enable() {
        lcl_setup();
        record->handle();
        lcl_teardown();
    }

    cldm_log("  %s", cldm_current_test.passed ? "pass" : "fail");
}


ssize_t cldm_test_collect(struct cldm_rbtree *restrict tree, struct cldm_elfmap const *restrict map) {
    struct cldm_testrec *record;

    for(size_t i = 0; i < map->strtab.size; i += strlen(map->strtab.addr + i) + 1) {
        if(strncmp(cldm_str_expand(cldm_testrec_prefix), map->strtab.addr + i, sizeof(cldm_str_expand(cldm_testrec_prefix)) - 1) == 0) {
            record = cldm_elf_testrec(map, map->strtab.addr + i);
            if(!record) {
                cldm_err("Could not map test record for %s", map->strtab.addr + i + sizeof(cldm_str_expand(cldm_testrec_prefix)) - 1);
                return -1;
            }

            cldm_rbtree_insert(tree, &record->rbnode, cldm_testrec_compare);
        }
    }

    return (ssize_t)cldm_rbtree_size(tree);
}

int cldm_test_invoke_each(struct cldm_rbtree const *restrict tests, struct cldm_elfmap const *restrict map, bool fail_fast) {
    size_t testidx;
    struct cldm_rbnode *iter;
    struct cldm_testrec const *record;

    void (*lcl_setup)(void);
    void (*lcl_teardown)(void);

    if(!cldm_rbtree_size(tests)) {
        return 0;
    }

    if(cldm_test_prologue(map, cldm_rbtree_size(tests), &lcl_setup, &lcl_teardown)) {
        return 1;
    }

    testidx = 0;
    cldm_rbtree_for_each(iter, tests) {
        record = cldm_testrec_get(iter, const);
        cldm_test_invoke(record, &testidx, cldm_rbtree_size(tests), lcl_setup, lcl_teardown);

        if(fail_fast && !cldm_current_test.passed) {
            break;
        }
    }

    return cldm_test_epilogue(map, cldm_rbtree_size(tests));
}

int cldm_test_invoke_specified(struct cldm_ht *restrict lookup_table, struct cldm_elfmap const *restrict map, bool fail_fast, size_t ntests, char **restrict files, size_t nfiles) {
    size_t testidx;
    struct cldm_testrec const *record;

    struct cldm_ht_entry *entry;
    struct cldm_rbht_node *node;
    struct cldm_rbnode *rbiter;
    char **iter;

    void (*lcl_setup)(void);
    void (*lcl_teardown)(void);

    if(!cldm_ht_size(lookup_table)) {
        return 0;
    }

    if(cldm_test_prologue(map, ntests, &lcl_setup, &lcl_teardown)) {
        return 1;
    }

    testidx = 0;
    cldm_for_each(iter, files, nfiles) {
        entry = cldm_ht_find(lookup_table, &cldm_ht_mkentry_str(*iter));
        node = cldm_container(entry, struct cldm_rbht_node, entry);

        /* Iterate between begin and end for each entry in lookup table */
        cldm_rbtree_for_each(rbiter, 0, node->begin, node->end) {
            record = cldm_testrec_get(rbiter, const);
            cldm_test_invoke(record, &testidx, ntests, lcl_setup, lcl_teardown);

            if(fail_fast && !cldm_current_test.passed) {
                break;
            }
        }
    }

    return cldm_test_epilogue(map, ntests);
}

void cldm_assert_internal(bool eval, char const *restrict expr, char const *restrict file, char const *restrict line) {
    ++cldm_test_log.total_assertions;

    if(eval) {
        return;
    }

    if(cldm_current_test.passed) {
        ++cldm_test_log.failed_tests;
        cldm_current_test.passed = false;
    }

    ++cldm_test_log.failed_assertions;

    cldm_rtassert(cldm_test_ensure_logcapacity(), "Could not increase test log size");

    snprintf(cldm_test_log.l_un.data[cldm_test_log.size], sizeof(cldm_test_log.l_un.data[cldm_test_log.size]),
             "| %s:%s: Assertion failure in %s:\n"
             "| '%s'", cldm_basename(file), line, cldm_current_test.name, expr);

    ++cldm_test_log.size;
}

#ifdef CLDM_HAS_GENERIC

#define generic_cmp_assertion(lhs, rhs, lexpand, rexpand, file, line, fmt, cmp)                     \
    do {                                                                                            \
        ++cldm_test_log.total_assertions;                                                           \
        if((lhs) cmp (rhs)) {                                                                       \
            return;                                                                                 \
        }                                                                                           \
        if(cldm_current_test.passed) {                                                              \
            ++cldm_test_log.failed_tests;                                                           \
            cldm_current_test.passed = false;                                                       \
        }                                                                                           \
        ++cldm_test_log.failed_assertions;                                                          \
        cldm_rtassert(cldm_test_ensure_logcapacity(), "Could not increase test log size");          \
        snprintf(cldm_test_log.l_un.data[cldm_test_log.size],                                       \
                 sizeof(cldm_test_log.l_un.data[cldm_test_log.size]),                               \
                 "| %s:%s: Assertion failure in %s:\n"                                              \
                 "| '%s " #cmp " %s' with expansion\n"                                              \
                 "| '" #fmt " " #cmp " " #fmt "'\n",                                                \
                 cldm_basename(file), line, cldm_current_test.name, lexpand, rexpand, lhs, rhs);    \
        ++cldm_test_log.size;                                                                       \
    } while(0)

#define cldm_genassert_defs4(op, opstr, suffix, type)                                                                                                                               \
    void cldm_assert_ ## opstr ## _ ## suffix(type l, type r, char const *restrict lexpand, char const *restrict rexpand, char const *restrict file, char const *restrict line) {   \
        generic_cmp_assertion(l, r, lexpand, rexpand, file, line, %suffix, op);                                                                                                     \
    }

#define cldm_genassert_defs6(op, opstr, suffix, type, ...)   \
    cldm_genassert_defs4(op, opstr, suffix, type)            \
    cldm_genassert_defs4(op, opstr, __VA_ARGS__)

#define cldm_genassert_defs8(op, opstr, suffix, type, ...)   \
    cldm_genassert_defs4(op, opstr, suffix, type)            \
    cldm_genassert_defs6(op, opstr, __VA_ARGS__)

#define cldm_genassert_defs10(op, opstr, suffix, type, ...)  \
    cldm_genassert_defs4(op, opstr, suffix, type)            \
    cldm_genassert_defs8(op, opstr, __VA_ARGS__)

#define cldm_genassert_defs(...) \
    cldm_cat_expand(cldm_genassert_defs, cldm_count(__VA_ARGS__))(__VA_ARGS__)

cldm_genassert_defs(==, eq, cldm_genassert_typelist)
cldm_genassert_defs(!=, ne, cldm_genassert_typelist)
cldm_genassert_defs(>,  gt, cldm_genassert_typelist)
cldm_genassert_defs(>=, ge, cldm_genassert_typelist)
cldm_genassert_defs(<,  lt, cldm_genassert_typelist)
cldm_genassert_defs(<=, le, cldm_genassert_typelist)

#endif /* CLDM_HAS_GENERIC */
