#include "cldm_algo.h"
#include "cldm_collect.h"
#include "cldm_log.h"
#include "cldm_mock.h"
#include "cldm_parallel.h"
#include "cldm_rbtree.h"
#include "cldm_test.h"
#include "cldm_thread.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <sys/types.h>

struct cldm_threadargs {
    bool fail_fast;
    unsigned id;
    struct cldm_testrec *volatile *records;
    pthread_barrier_t *barrier;
    pthread_rwlock_t *fail_lock;
};

struct cldm_extended_threadargs {
    struct cldm_threadargs *thrdargs;
    struct cldm_elfmap const *map;
    struct cldm_args const *args;
};
static struct cldm_threadargs cldm_threadargs[CLDM_MAX_THREADS];
static unsigned long long thread_ntests[CLDM_MAX_THREADS];
static unsigned long long thread_startidx[CLDM_MAX_THREADS];
static struct cldm_auxprocs auxprocs;
static bool early_abort;

static void *cldm_thread_run(void *data) {
    struct cldm_threadargs *thrdargs;
    bool init_successful;
    bool abort;

    thrdargs = data;
    abort = false;

    /* Initialize test logs while waiting */
    init_successful = cldm_test_init(thrdargs->id);

    /* Wait for setup to finish */
    pthread_barrier_wait(thrdargs->barrier);

    if(!init_successful) {
        return 0;
    }

    for(unsigned i = 0; i < thread_ntests[thrdargs->id]; i++) {
        if(!cldm_test_run(thrdargs->id, &(*thrdargs->records)[thread_startidx[thrdargs->id] + i], &auxprocs, thrdargs->fail_fast)) {
            if(thrdargs->fail_fast) {
                cldm_rwlock_wrguard(thrdargs->fail_lock) {
                    early_abort = true;
                }
            }
            break;
        }

        if(thrdargs->fail_fast) {
            cldm_rwlock_rdguard(thrdargs->fail_lock) {
                abort = early_abort;
            }
            if(abort) {
                break;
            }
        }
    }

    cldm_test_flush(thrdargs->id);

    pthread_barrier_wait(thrdargs->barrier);
    /* Parallel reduction */
    for(unsigned i = cldm_flp232(cldm_jobs); i; i >>= 1) {
        if(thrdargs->id < i) {
            cldm_test_reduce(thrdargs->id, i);
        }
        pthread_barrier_wait(thrdargs->barrier);
    }

    /* Delay cleanup for master thread to ensure io_lock in cldm_test.c is not
     * destroyed prematurely */
    if(thrdargs->id) {
        cldm_test_free(thrdargs->id);
    }

    return 0;
}

static void *cldm_parallel_collect_and_run(void *data) {
    struct cldm_extended_threadargs *ethrdargs;
    struct cldm_rbnode *rbnode;
    struct cldm_rbtree tree;
    unsigned testidx;
    size_t startidx;
    ssize_t ntests;

    ethrdargs = data;

    /* Collect tests */
    if(ethrdargs->args->nposparams) {
        ntests = cldm_collect_from(&tree, ethrdargs->map, ethrdargs->args->posparams, ethrdargs->args->nposparams);
    }
    else {
        ntests = cldm_collect(&tree, ethrdargs->map);
    }

    if(ntests < 0) {
        /* Allow all threads to run, but do nothing, to bypass barrier */
        for(unsigned i = 0; i < cldm_arrsize(cldm_threadargs); i++) {
            thread_ntests[i] = 0;
        }
        goto epilogue;
    }

    cldm_test_register((size_t)ntests, ethrdargs->args->verbose);
    *ethrdargs->thrdargs->records = malloc(ntests * sizeof(**ethrdargs->thrdargs->records));

    if(!*ethrdargs->thrdargs->records) {
        cldm_err("Could not allocate memory for test records");
        goto epilogue;
    }

    testidx = 0u;
    cldm_rbtree_for_each(rbnode, &tree) {
        (*ethrdargs->thrdargs->records)[testidx++] = *cldm_testrec_get(rbnode);
    }

    /* Distribute across threads */
    startidx = 0u;
    for(unsigned i = 0; i < ethrdargs->args->jobs; i++) {
        thread_ntests[i] = ntests / ethrdargs->args->jobs + (i < (ntests % ethrdargs->args->jobs));
        thread_startidx[i] = startidx;
        startidx += thread_ntests[i];
    }

    /* Collect auxiliary procedures */
    cldm_collect_auxprocs(&auxprocs, ethrdargs->map);

    /* Issue global setup while other threads are blocked */
    cldm_mock_global() {
        cldm_mock_enable() {
            auxprocs.global_setup();
        }
    }
epilogue:
    return cldm_thread_run(ethrdargs->thrdargs);
}


int cldm_parallel_run(struct cldm_elfmap const *restrict map, struct cldm_args const *restrict args) {
    pthread_barrier_t barrier;
    pthread_rwlock_t fail_lock;
    struct cldm_testrec *records;
    int err;

    if(args->jobs < 2 || args->jobs > CLDM_MAX_THREADS) {
        cldm_err("Parallel execution expects number of threads to be in range 2-%d", CLDM_MAX_THREADS);
        return 1;
    }

    cldm_thread_init(args->jobs);

    records = 0;

    err = pthread_barrier_init(&barrier, 0, args->jobs);
    if(err) {
        cldm_err("Could not initialize barrier: %s", strerror(err));
        return 1;
    }

    err = pthread_rwlock_init(&fail_lock, 0);
    if(err) {
        cldm_err("Could not initialize rwlock: %s", strerror(err));
        pthread_barrier_destroy(&barrier);
        return 1;
    }

    /* Start test collection on thread 1 */
    cldm_threadargs[1] = (struct cldm_threadargs) {
        .fail_fast = args->fail_fast,
        .id = 1u,
        .records = &records,
        .barrier = &barrier,
        .fail_lock = &fail_lock
    };

    err = pthread_create(&cldm_threads[0], 0, cldm_parallel_collect_and_run, &(struct cldm_extended_threadargs) { .thrdargs = &cldm_threadargs[1], .map = map, .args = args });
    cldm_rtassert(!err, "Spawning thread 1 failed: %s", strerror(err));

    /* Spawn pure worker threads */
    for(unsigned i = 2; i < args->jobs; i++) {
        cldm_threadargs[i] = (struct cldm_threadargs) {
            .fail_fast = args->fail_fast,
            .id = i,
            .records = &records,
            .barrier = &barrier,
            .fail_lock = &fail_lock
        };
        err = pthread_create(&cldm_threads[i - 1], 0, cldm_thread_run, &cldm_threadargs[i]);
        cldm_rtassert(!err, "Spawning thread %u failed: %s", i, strerror(err));
    }

    /* Start master thread processing */
    cldm_threadargs[0] = (struct cldm_threadargs) {
        .fail_fast = args->fail_fast,
        .id = 0u,
        .records = &records,
        .barrier = &barrier,
        .fail_lock = &fail_lock
    };
    cldm_thread_run(&cldm_threadargs[0]);

    /* Join and clean up */
    for(unsigned i = 0; i < args->jobs - 1; i++) {
        err = pthread_join(cldm_threads[i], 0);
        if(err) {
            cldm_err("Could not join thread %u: %s", i + 1, strerror(err));
        }
    }
    cldm_mock_global() {
        cldm_mock_enable() {
            auxprocs.global_teardown();
        }
    }

    err = cldm_test_summary();

    /* Clean up test state for master thread */
    cldm_test_free(0);

    if(records) {
        free(records);
    }

    pthread_rwlock_destroy(&fail_lock);
    pthread_barrier_destroy(&barrier);

    return err;
}

