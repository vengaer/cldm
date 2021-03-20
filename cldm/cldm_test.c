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

static inline char const *cldm_test_extract_name(char const *proc) {
    return proc + sizeof(cldm_str_expand(cldm_test_proc_prefix)) - 1;
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

static void cldm_test_summary(size_t ntests) {
    char *iter;
    if(cldm_test_log.failed_tests) {
        cldm_log_stream(cldm_stderr, "\n%llu/%llu assertions failed across %llu tests", cldm_test_log.failed_assertions, cldm_test_log.total_assertions, cldm_test_log.failed_tests);
        cldm_for_each(iter, ((char *)cldm_test_log.l_un.addr), cldm_test_log.size * sizeof(*cldm_test_log.l_un.data), sizeof(*cldm_test_log.l_un.data)) {
            cldm_log_stream(cldm_stderr, "\n%s", iter);
        }
        return;
    }
    cldm_log("\nSuccessfully finished %llu assertions across %zu tests", cldm_test_log.total_assertions, ntests);
}

ssize_t cldm_test_collect(char *restrict buffer, struct cldm_elfmap const *restrict map, size_t bufsize) {
    ssize_t ntests = 0;
    ssize_t strtab_size;
    ssize_t symsize;
    char strtab[CLDM_PAGE_SIZE];
    ssize_t offset;

    strtab_size = cldm_elf_read_strtab(map, strtab, ".strtab", sizeof(strtab));
    if(strtab_size < 0) {
        cldm_err("Could not read strtab");
        return -1;
    }

    offset = 0;
    for(ssize_t i = 0; i < strtab_size; i += cldm_ntbslen(strtab + i) + 1) {
        if(cldm_ntbsncmp(cldm_str_expand(cldm_test_proc_prefix), strtab + i, sizeof(cldm_str_expand(cldm_test_proc_prefix)) - 1) == 0) {
            symsize = cldm_ntbscpy(buffer + offset, strtab + i, bufsize - offset);
            if(symsize < 0 || (size_t)(offset + symsize + 1) >= bufsize) {
                cldm_err("Test %lld would cause internal buffer overflow", (long long)i);
                return -E2BIG;
            }
            offset += symsize;
            buffer[offset++] = ';';
            buffer[offset] = '\0';
            ++ntests;
        }
    }

    return ntests;
}

int cldm_test_invoke_each(struct cldm_elfmap const *restrict map, char const *restrict tests, size_t ntests) {
    void (*test)(void);
    char *iter;
    unsigned long long testidx;

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
    cldm_for_each_word(iter, tests, ';') {
        cldm_test_set(iter);
        test = cldm_elf_func(map, iter);
        if(!test) {
            cldm_warn("Could not locate address of %s in .test segment", iter);
            continue;
        }

        cldm_log_raw("[Running %s] (%llu/%zu)", cldm_test_extract_name(iter), ++testidx, ntests);
        lcl_setup();
        test();
        lcl_teardown();

        cldm_log("  %s", cldm_current_test.passed ? "pass" : "fail");
    }

    glob_teardown();

    cldm_test_summary(ntests);
    cldm_test_free();

    return !!cldm_test_log.failed_tests;
}


void cldm_test_assertion(char const *restrict expr, char const *restrict file, char const *restrict line, bool result) {
    void *addr;
    size_t new_cap;
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

    if(cldm_test_log.size  + 1 >= cldm_test_log.capacity) {
        new_cap = 2 * cldm_test_log.capacity * sizeof(*cldm_test_log.l_un.data);
        addr = realloc(cldm_test_log.l_un.addr, new_cap);
        cldm_rtassert(addr, "Could not allocate chunk of %zu bytes", new_cap);
        cldm_test_log.l_un.addr = addr;
    }

    snprintf(cldm_test_log.l_un.data[cldm_test_log.size], sizeof(cldm_test_log.l_un.data[cldm_test_log.size]),
             "| %s:%s: Assertion failure in %s:\n"
             "| '%s'", basename, line, cldm_test_extract_name(cldm_current_test.name), expr);

    ++cldm_test_log.size;
}
