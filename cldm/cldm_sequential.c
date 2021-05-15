#include "cldm_auxprocs.h"
#include "cldm_log.h"
#include "cldm_mock.h"
#include "cldm_rbtree.h"
#include "cldm_sequential.h"
#include "cldm_test.h"
#include "cldm_testrec.h"
#include "cldm_thread.h"

#include <stddef.h>

#include <sys/types.h>

int cldm_sequential_run(struct cldm_elfmap const *restrict map, struct cldm_args const *restrict args) {
    struct cldm_auxprocs auxprocs;
    struct cldm_testrec const *record;
    struct cldm_rbnode *iter;
    struct cldm_rbtree tree;
    ssize_t ntests;
    int status;

    if(args->jobs != 1) {
        cldm_err("Sequential execution requires a single thread");
        return 1;
    }

    cldm_thread_init(args->jobs);

    if(args->nposparams) {
        ntests = cldm_collect_from(&tree, map, args->posparams, args->nposparams);
    }
    else {
        ntests = cldm_collect(&tree, map);
    }

    if(ntests < 0) {
        return 1;
    }

    cldm_test_register((size_t)ntests, args->verbose);
    cldm_collect_auxprocs(&auxprocs, map);


    if(!cldm_test_init(0)) {
        return 1;
    }

    cldm_mock_global() {
        cldm_mock_enable() {
            auxprocs.global_setup();
        }
    }

    cldm_rbtree_for_each(iter, &tree) {
        record = cldm_testrec_get(iter, const);
        if(!cldm_test_run(0, record, &auxprocs, args->fail_fast)) {
            break;
        }
    }

    cldm_mock_global() {
        cldm_mock_enable() {
            auxprocs.global_teardown();
        }
    }

    cldm_test_flush(0);

    status = cldm_test_summary();

    cldm_test_free(0);
    return status;
}
