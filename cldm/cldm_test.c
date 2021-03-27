#include "cldm_dl.h"
#include "cldm_dlwrp.h"
#include "cldm_io.h"
#include "cldm_log.h"
#include "cldm_mem.h"
#include "cldm_ntbs.h"
#include "cldm_rtassert.h"
#include "cldm_test.h"

enum { CLDM_ASSERTION_LEN = 256 };
enum { CLDM_LOG_INITIAL_CAP = 32 };
enum { CLDM_RUNBUF_SIZE = 64 };
enum { CLDM_IDXBUF_SIZE = 32 };

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

static bool cldm_test_ensure_logcapacity(void) {
    void *addr;
    size_t new_cap;

    if(cldm_test_log.size  + 1 >= cldm_test_log.capacity) {
        new_cap = 2 * cldm_test_log.capacity * sizeof(*cldm_test_log.l_un.data);
        addr = realloc(cldm_test_log.l_un.addr, new_cap);
        cldm_rtassert(addr, "Could not allocate chunk of %zu bytes", new_cap);
        if(!addr) {
            free(cldm_test_log.l_un.addr);
            return false;
        }
        cldm_test_log.l_un.addr = addr;
        cldm_test_log.capacity = new_cap;
    }

    return true;
}

static inline void cldm_test_free(void) {
    free(cldm_test_log.l_un.addr);
}

static void cldm_test_summary(size_t ntests) {
    char *iter;
    if(cldm_test_log.failed_tests) {
        cldm_log_stream(cldm_stderr, "\n%llu/%llu assertions failed across %llu test%s",
                        cldm_test_log.failed_assertions, cldm_test_log.total_assertions, cldm_test_log.failed_tests, cldm_test_log.failed_tests == 1 ? "" : "s");

        cldm_for_each(iter, ((char *)cldm_test_log.l_un.addr), cldm_test_log.size * sizeof(*cldm_test_log.l_un.data), sizeof(*cldm_test_log.l_un.data)) {
            cldm_log_stream(cldm_stderr, "\n%s", iter);
        }
        return;
    }
    cldm_log("\nSuccessfully finished %llu assertions across %zu test%s", cldm_test_log.total_assertions, ntests, ntests == 1 ? "" : "s");
}

ssize_t cldm_test_collect(cldm_rbtree *restrict tree, struct cldm_elfmap const *restrict map) {
    ssize_t ntests = 0;
    struct cldm_testrec *record;

    for(size_t i = 0; i < map->strtab.size; i += cldm_ntbslen(map->strtab.addr + i) + 1) {
        if(cldm_ntbsncmp(cldm_str_expand(cldm_testrec_prefix), map->strtab.addr + i, sizeof(cldm_str_expand(cldm_testrec_prefix)) - 1) == 0) {
            record = cldm_elf_testrec(map, map->strtab.addr + i);
            if(!record) {
                cldm_err("Could not map test record for %s", map->strtab.addr + i + sizeof(cldm_str_expand(cldm_testrec_prefix)) - 1);
                return -1;
            }

            cldm_rtassert(cldm_rbtree_insert(tree, &record->rbnode, cldm_testrec_compare));
            ++ntests;
        }
    }

    return ntests;
}

int cldm_test_invoke_each(cldm_rbtree const *restrict tests, struct cldm_elfmap const *restrict map, size_t ntests) {
    char runbuf[CLDM_RUNBUF_SIZE];
    char idxbuf[CLDM_IDXBUF_SIZE];
    unsigned long long testidx;
    struct cldm_rbnode *iter;
    struct cldm_testrec const *record;

    void (*lcl_setup)(void);
    void (*lcl_teardown)(void);
    void (*glob_setup)(void);
    void (*glob_teardown)(void);

    if(!ntests) {
        return 0;
    }

    cldm_test_init();

    lcl_setup = cldm_test_local_setup(map);
    lcl_teardown= cldm_test_local_teardown(map);
    glob_setup = cldm_test_global_setup(map);
    glob_teardown = cldm_test_global_teardown(map);
    cldm_log("");

    glob_setup();

    testidx = 0;
    cldm_rbtree_for_each(iter, tests) {
        record = cldm_testrec_get(iter, const);
        cldm_test_set(record->name);

        snprintf(runbuf, sizeof(runbuf), "[Running %s]", record->name);
        snprintf(idxbuf, sizeof(idxbuf), "(%llu/%zu)", ++testidx, ntests);

        cldm_log_raw("%-40s %-10s", runbuf, idxbuf);
        lcl_setup();
        record->handle();
        lcl_teardown();

        cldm_log("  %s", cldm_current_test.passed ? "pass" : "fail");
    }

    glob_teardown();

    cldm_test_summary(ntests);
    cldm_test_free();

    return !!cldm_test_log.failed_tests;
}


void cldm_test_assertion(char const *restrict expr, char const *restrict file, char const *restrict line, bool result) {
    ++cldm_test_log.total_assertions;

    if(result) {
        return;
    }

    char const *basename = cldm_ntbscrchr(file, '/');
    basename = basename ? basename + 1 : file;
    if(cldm_current_test.passed) {
        ++cldm_test_log.failed_tests;
        cldm_current_test.passed = false;
    }

    ++cldm_test_log.failed_assertions;
    cldm_rtassert(cldm_test_ensure_logcapacity(), "Could not increase test log size");

    snprintf(cldm_test_log.l_un.data[cldm_test_log.size], sizeof(cldm_test_log.l_un.data[cldm_test_log.size]),
             "| %s:%s: Assertion failure in %s:\n"
             "| '%s'", basename, line, cldm_current_test.name, expr);

    ++cldm_test_log.size;
}
