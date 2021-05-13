#include "cldm_collect.h"
#include "cldm_hash.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_ntbs.h"
#include "cldm_rbtree.h"
#include "cldm_runner.h"
#include "cldm_test.h"

#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

int cldm_collect_and_run(struct cldm_elfmap const *restrict map, bool fail_fast, char **restrict files, size_t nfiles) {
    struct cldm_rbtree tests;

    tests = cldm_rbtree_init();
    if(nfiles) {
        if(cldm_collect_from(&tests, map, files, nfiles) < 0) {
            cldm_err("Error collecting tests");
            return -1;
        }
    }
    else {
        if(cldm_collect(&tests, map) < 0) {
            cldm_err("Error collecting tests");
            return -1;
        }
    }

    return cldm_test_invoke_each(&tests, map, fail_fast);
}
