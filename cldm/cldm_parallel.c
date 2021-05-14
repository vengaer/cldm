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
    struct cldm_auxprocs volatile *auxprocs;
    pthread_barrier_t *barrier;
};

struct cldm_extended_threadargs {
    struct cldm_threadargs *thrdargs;
    struct cldm_elfmap const *map;
    struct cldm_args const *args;
};
static struct cldm_threadargs cldm_threadargs[CLDM_MAX_THREADS];
static unsigned long long ntest_per_thread[CLDM_MAX_THREADS];
static unsigned long long startidx_per_thread[CLDM_MAX_THREADS];

static void *cldm_thread_run(void *data) {
    struct cldm_threadargs *thrdargs;
    struct cldm_auxprocs auxprocs;
    unsigned long long startidx;
    unsigned long long ntests;
    bool init_successful;
    thrdargs = data;

    /* Initialize test logs while waiting */
    init_successful = cldm_test_init(thrdargs->id);

    /* Wait for setup to finish */
    pthread_barrier_wait(thrdargs->barrier);

    if(!init_successful) {
        return 0;
    }

    ntests = ntest_per_thread[thrdargs->id];
    startidx = startidx_per_thread[thrdargs->id];

    auxprocs = *thrdargs->auxprocs;

    for(unsigned i = 0; i < ntests; i++) {
        if(!cldm_test_run(thrdargs->id, &(*thrdargs->records)[startidx + i], &auxprocs, thrdargs->fail_fast)) {
            break;
        }
    }

    cldm_test_flush(thrdargs->id);

    pthread_barrier_wait(thrdargs->barrier);
    /* Parallel reduction */
    for(unsigned i = cldm_clp232(cldm_jobs) >> 1u; i; i >>= 1) {
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
    struct cldm_auxprocs auxprocs;
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
        cldm_err("Test collection failed");
        /* Allow all threads to run, but do nothing, to bypass barrier */
        for(unsigned i = 0; i < cldm_arrsize(cldm_threadargs); i++) {
            ntest_per_thread[i] = 0;
        }
        goto epilogue;
    }

    cldm_test_register_total((size_t)ntests);
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
        ntest_per_thread[i] = ntests / ethrdargs->args->jobs + (i < (ntests % ethrdargs->args->jobs));
        startidx_per_thread[i] = startidx;
        startidx += ntest_per_thread[i];
    }

    /* Collect auxiliary procedures */
    cldm_collect_auxprocs(&auxprocs, ethrdargs->map);
    *ethrdargs->thrdargs->auxprocs = auxprocs;

    /* Issue global setup while other threads are blocked */
    /* TODO: mock all threads */
    cldm_mock_enable() {
        ethrdargs->thrdargs->auxprocs->global_setup();
    }
epilogue:
    return cldm_thread_run(ethrdargs->thrdargs);
}

int cldm_parallel_run(struct cldm_elfmap const *restrict map, struct cldm_args const *restrict args) {
    pthread_barrier_t barrier;
    struct cldm_auxprocs auxprocs;
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

    /* Start test collection on thread 1 */
    cldm_threadargs[1] = (struct cldm_threadargs) {
        .fail_fast = args->fail_fast,
        .id = 1u,
        .records = &records,
        .auxprocs = &auxprocs,
        .barrier = &barrier
    };

    err = pthread_create(&cldm_threads[0], 0, cldm_parallel_collect_and_run, &(struct cldm_extended_threadargs) { .thrdargs = &cldm_threadargs[1], .map = map, .args = args });
    cldm_rtassert(!err, "Spawning thread 1 failed: %s", strerror(err));

    /* Spawn pure worker threads */
    for(unsigned i = 2; i < args->jobs; i++) {
        cldm_threadargs[i] = (struct cldm_threadargs) {
            .fail_fast = args->fail_fast,
            .id = i,
            .records = &records,
            .auxprocs = &auxprocs,
            .barrier = &barrier
        };
        err = pthread_create(&cldm_threads[i - 1], 0, cldm_thread_run, &cldm_threadargs[i]);
        cldm_rtassert(!err, "Spawning thread %u failed: %s", i, strerror(err));
    }

    /* Start master thread processing */
    cldm_threadargs[0] = (struct cldm_threadargs) {
        .fail_fast = args->fail_fast,
        .id = 0u,
        .records = &records,
        .auxprocs = &auxprocs,
        .barrier = &barrier
    };
    cldm_thread_run(&cldm_threadargs[0]);

    /* Join and clean up */
    for(unsigned i = 0; i < args->jobs - 1; i++) {
        err = pthread_join(cldm_threads[i], 0);
        if(err) {
            cldm_err("Could not join thread %u: %s", i + 1, strerror(err));
        }
    }
    /* TODO: mock all threads */
    cldm_mock_enable() {
        auxprocs.global_teardown();
    }

    err = cldm_test_summary();

    /* Clean up test state for master thread */
    cldm_test_free(0);

    if(records) {
        free(records);
    }

    pthread_barrier_destroy(&barrier);

    return err;
}

