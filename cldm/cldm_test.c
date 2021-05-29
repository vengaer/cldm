#include "cldm_algo.h"
#include "cldm_byteseq.h"
#include "cldm_cache.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_mock.h"
#include "cldm_rtassert.h"
#include "cldm_test.h"
#include "cldm_thread.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

enum { CLDM_TESTBUFFER_SIZE = 16 };
enum { CLDM_LOG_INITIAL_CAP = 32 };
enum { CLDM_TEST_PRINT_WIDTH = 64 };
enum { CLDM_TEST_PRINT_VERBOSE_ADJUST = 32 };
enum { CLDM_INDEX_PRINT_WIDTH = 10 };
enum { CLDM_MAX_EXPAND_SIZE = 256 };
enum { CLDM_ASSERTION_SIZE = 1024 };

struct cldm_testlog {
    union {
        char (*buffer)[CLDM_ASSERTION_SIZE];
        void *addr;
    } l_un;
    size_t size;
    size_t capacity;
    unsigned long long failed_assertions;
    unsigned long long failed_tests;
    unsigned long long total_assertions;
};

struct cldm_teststats {
    char const *name;
    bool pass;
    bool fatal_error;
};

struct cldm_testbuffer {
    struct {
        char const *name;
        char const *file;
        int namepad;
        bool pass;
    } stats[CLDM_TESTBUFFER_SIZE];
    unsigned size;
};

static cldm_cachealign(struct cldm_testlog) testlogs[CLDM_MAX_THREADS];
static cldm_cachealign(struct cldm_teststats) current_tests[CLDM_MAX_THREADS];
static cldm_cachealign(struct cldm_testbuffer) buffered_tests[CLDM_MAX_THREADS];

static pthread_mutex_t io_lock;
static size_t io_index;
static size_t io_total;
static bool io_verbose;

#define record_failed_assertion(thread_id, ...)                                                         \
    do {                                                                                                \
        if(current_tests[thread_id].data.pass) {                                                        \
            ++testlogs[thread_id].data.failed_tests;                                                    \
            current_tests[thread_id].data.pass = false;                                                 \
        }                                                                                               \
        ++testlogs[thread_id].data.failed_assertions;                                                   \
        if(!cldm_test_ensure_capacity(thread_id)) {                                                     \
            current_tests[thread_id].data.fatal_error = true;                                           \
        }                                                                                               \
        else {                                                                                          \
            snprintf(testlogs[thread_id].data.l_un.buffer[testlogs[thread_id].data.size],               \
                     sizeof(testlogs[thread_id].data.l_un.buffer[testlogs[thread_id].data.size]),       \
                     __VA_ARGS__);                                                                      \
            ++testlogs[thread_id].data.size;                                                            \
        }                                                                                               \
    } while(0)

static bool cldm_test_ensure_capacity(unsigned thread_id) {
    void *addr;
    size_t newcap;

    if(testlogs[thread_id].data.size + 1 < testlogs[thread_id].data.capacity) {
        return true;
    }
    newcap = 2 * testlogs[thread_id].data.capacity;
    addr = realloc(testlogs[thread_id].data.l_un.addr, newcap * sizeof(*testlogs[thread_id].data.l_un.buffer));
    if(!addr) {
        return false;
    }
    testlogs[thread_id].data.l_un.addr = addr;
    testlogs[thread_id].data.capacity = newcap;
    return true;
}

static void cldm_test_print(unsigned thread_id, struct cldm_testbuffer const *buffer) {
    int l;
    int idxpad;
    cldm_mutex_guard(&io_lock) {
        for(unsigned i = 0; i < buffer->size; i++) {
            ++io_index;
            l = cldm_strlitlen("(/)") + cldm_ndigits(io_index) + cldm_ndigits(io_total);
            idxpad = (l <= CLDM_INDEX_PRINT_WIDTH) * (CLDM_INDEX_PRINT_WIDTH - l);
            if(io_verbose) {
                cldm_log("[Thread %u:%-*s running %s::%s]%-*s (%zu/%zu)%-*s  %s",
                        thread_id, 2 - cldm_ndigits(thread_id), "", buffer->stats[i].file, buffer->stats[i].name,
                        buffer->stats[i].namepad, "",
                        io_index, io_total, idxpad, "",
                        buffer->stats[i].pass ? "pass" : "fail");
            }
            else {
                cldm_log("[Running %s]%-*s (%zu/%zu)%-*s  %s",
                        buffer->stats[i].name, buffer->stats[i].namepad, "",
                        io_index, io_total, idxpad, "",
                        buffer->stats[i].pass ? "pass" : "fail");
            }
        }
    }

}

static void cldm_test_schedule_print(unsigned thread_id, char const *file) {
    struct cldm_testbuffer *buffer;
    int l0;
    int l1;

    buffer = &buffered_tests[thread_id].data;
    if(buffer->size >= cldm_arrsize(buffer->stats)) {
        cldm_test_print(thread_id, buffer);
        buffer->size = 0;
    }

    buffer->stats[buffer->size].name = current_tests[thread_id].data.name;
    buffer->stats[buffer->size].file = cldm_basename(file);
    buffer->stats[buffer->size].pass = current_tests[thread_id].data.pass;
    l0 = strlen(current_tests[thread_id].data.name) + cldm_strlitlen("[Running ]") +
            io_verbose * (strlen(buffer->stats[buffer->size].file) + cldm_strlitlen("Thread :"));
    l1 = CLDM_TEST_PRINT_WIDTH + io_verbose * CLDM_TEST_PRINT_VERBOSE_ADJUST;
    buffer->stats[buffer->size].namepad = (l0 <= l1) * (l1 - l0);
    ++buffer->size;
}

void cldm_test_register(size_t n, bool verbose) {
    cldm_log("Collected %zu tests", (size_t)n);
    io_total = n;
    io_verbose = verbose;
}

int cldm_test_summary(void) {
    if(testlogs[0].data.failed_tests) {
        cldm_log_stream(cldm_stderr, "\n%llu/%llu assertions failed across %llu/%zu test%s",
                        testlogs[0].data.failed_assertions, testlogs[0].data.total_assertions,
                        testlogs[0].data.failed_tests, io_total, io_total == 1u ? "" : "s");

        for(unsigned i = 0; i < cldm_jobs; i++) {
            for(unsigned j = 0; j < testlogs[i].data.size; j++) {
                cldm_log_stream(cldm_stderr, "\n%s", testlogs[i].data.l_un.buffer[j]);
            }
        }
        return 1;
    }

    cldm_log("\nSuccessfully finished %llu assertions across %zu test%s", testlogs[0].data.total_assertions, io_total, io_total == 1u ? "" : "s");
    return 0;
}

bool cldm_test_init(unsigned thread_id) {
    struct cldm_testlog *log;
    int err;
    log = &testlogs[thread_id].data;

    *log = (struct cldm_testlog) {
        .l_un.addr = malloc(CLDM_LOG_INITIAL_CAP * sizeof(*log->l_un.buffer)),
        .capacity = CLDM_LOG_INITIAL_CAP
    };

    if(!log->l_un.buffer) {
        cldm_err("Thread %u: could not allocate chunk of %d bytes for test log", thread_id, CLDM_LOG_INITIAL_CAP);
        return false;
    }

    buffered_tests[thread_id].data.size = 0;

    if(!thread_id) {
        err = cldm_mutex_init(&io_lock, 0);
        if(err) {
            cldm_err("Could not initialize io mutex: %s", strerror(err));
            return false;
        }
    }

    return true;
}

void cldm_test_free(unsigned thread_id) {
    int err;
    free(testlogs[thread_id].data.l_un.addr);
    if(!thread_id) {
        err = cldm_mutex_destroy(&io_lock);
        if(err) {
            cldm_warn("Could not destroy io mutex: %s", strerror(err));
        }
    }
}

bool cldm_test_run(unsigned thread_id, struct cldm_testrec const *restrict record, struct cldm_auxprocs const *restrict auxprocs, bool fail_fast) {
    current_tests[thread_id].data = (struct cldm_teststats) {
        .name = record->name,
        .pass = true,
        .fatal_error = false
    };

    cldm_mock_enable() {
        auxprocs->local_setup();
        if(record->runinfo.setup) {
            record->runinfo.setup();
        }
        record->handle();
        if(record->runinfo.teardown) {
            record->runinfo.teardown();
        }
        auxprocs->local_teardown();
    }

    cldm_test_schedule_print(thread_id, record->file);

    return !current_tests[thread_id].data.fatal_error && (!fail_fast || (fail_fast && current_tests[thread_id].data.pass));
}

void cldm_test_flush(unsigned thread_id) {
    cldm_test_print(thread_id, &buffered_tests[thread_id].data);
}

void cldm_test_reduce(unsigned thread_id, unsigned offset) {
    testlogs[thread_id].data.failed_assertions += testlogs[thread_id + offset].data.failed_assertions;
    testlogs[thread_id].data.failed_tests += testlogs[thread_id + offset].data.failed_tests;
    testlogs[thread_id].data.total_assertions += testlogs[thread_id + offset].data.total_assertions;
}

void cldm_assert_true(bool eval, char const *restrict expand, char const *restrict file, char const *restrict line) {
    unsigned thread_id = cldm_thread_id();
    ++testlogs[thread_id].data.total_assertions;
    if(eval) {
        return;
    }

    record_failed_assertion(thread_id,
                            "| %s:%s: Assertion failure in %s:\n"
                            "| '%s'", cldm_basename(file), line,
                            current_tests[thread_id].data.name,
                            expand);
}

void cldm_assert_streq(char const *restrict l, char const *restrict r, long long n, char const *restrict lname, char const *restrict rname, char const *restrict file, char const *restrict line) {
    unsigned thread_id = cldm_thread_id();
    size_t diffoffset;
    char const *ll;
    char const *rr;
    int expandsize;


    ++testlogs[thread_id].data.total_assertions;

    if((n < 0 && strcmp(l, r) == 0) || (n >= 0 && strncmp(l, r, n) == 0)) {
        return;
    }

    ll = l;
    rr = r;
    while(*ll++ == *rr++);
    diffoffset = cldm_strlitlen("string n is '") + ll - l - 1;

    #define expandsuffix(str)   strlen(str) > CLDM_MAX_EXPAND_SIZE ? "..." : ""
    if(n < 0) {
        expandsize = CLDM_MAX_EXPAND_SIZE;
        record_failed_assertion(thread_id,
                                "| %s:%s: Assertion failure in %s:\n"
                                "| expected '%s' to be equal to '%s' where \n"
                                "| string 0 is '%.*s%s' and \n"
                                "| string 1 is '%.*s%s'\n"
                                "  %-*s^\n",
                                cldm_basename(file), line, current_tests[thread_id].data.name,
                                lname, rname,
                                expandsize, l, expandsuffix(l),
                                expandsize, r, expandsuffix(r),
                                (int)diffoffset, "");
    }
    else {
        expandsize = n > CLDM_MAX_EXPAND_SIZE ? CLDM_MAX_EXPAND_SIZE : n;
        record_failed_assertion(thread_id,
                                "| %s:%s: Assertion failure in %s:\n"
                                "| expected %lld initial bytes of '%s' and '%s' to be equal where\n"
                                "| string 0 is '%.*s%s' and \n"
                                "| string 1 is '%.*s%s'\n"
                                "  %-*s^\n",
                                cldm_basename(file), line, current_tests[thread_id].data.name,
                                n, lname, rname,
                                expandsize, l, expandsuffix(l),
                                expandsize, r, expandsuffix(r),
                                (int)diffoffset, "");
    }
    #undef expandsuffix

}

#ifdef CLDM_HAS_GENERIC

#define generic_cmp_assertion(l, r, lexpand, rexpand, file, line, fmt, cmp)                             \
    do {                                                                                                \
        unsigned cldm_cat_expand(cldm_gcd_tid,__LINE__) = cldm_thread_id();                             \
        ++testlogs[cldm_cat_expand(cldm_gcd_tid,__LINE__)].data.total_assertions;                       \
        if(!((l) cmp (r))) {                                                                            \
            record_failed_assertion(cldm_thread_id(),                                                   \
                                    "| %s:%s: Assertion failure in %s:\n"                               \
                                    "| '%s " #cmp " %s' with expansion\n"                               \
                                    "| '" #fmt " " #cmp " " #fmt "'\n",                                 \
                                    cldm_basename(file), line,                                          \
                                    current_tests[cldm_cat_expand(cldm_gcd_tid,__LINE__)].data.name,    \
                                    lexpand, rexpand, l, r);                                            \
        }                                                                                               \
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
