#include "cldm_test.h"
#include "cldm_log.h"
#include "cldm_mem.h"

#include <sys/types.h>

int main(int argc, char *argv[argc + 1]) {
    char buffer[CLDM_PAGE_SIZE];
    ssize_t ntests;

    ntests = cldm_test_collect(buffer, argv[0], sizeof(buffer));
    if(ntests < 0) {
        cldm_err("Error collecting tests");
        return 1;
    }

    cldm_log("Collected %lld tests", (long long)ntests);

    cldm_test_invoke_each(buffer, (size_t)ntests);


    return 0;
}
