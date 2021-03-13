#include "cldm_test.h"
#include "cldm_io.h"
#include "cldm_log.h"
#include "cldm_mem.h"

#include <sys/types.h>

int main(int argc, char *argv[argc + 1]) {
    char buffer[CLDM_PAGE_SIZE];
    ssize_t ntests;
    int status;

    status = 1;

    if(cldm_io_capture_stdout()) {
        cldm_err("Error redirecting stdout");
        goto epilogue;
    }

    if(cldm_io_capture_stderr()) {
        cldm_err("Error redirecting stderr");
        goto epilogue;
    }

    ntests = cldm_test_collect(buffer, argv[0], sizeof(buffer));
    if(ntests < 0) {
        cldm_err("Error collecting tests");
        goto epilogue;
    }

    cldm_log("Collected %lld tests", (long long)ntests);

    if(cldm_test_invoke_each(buffer, (size_t)ntests)) {
        cldm_err("Error while running tests");
        goto epilogue;
    }

    cldm_log("\nCaptured stdout:");
    if(cldm_io_dump_captured_stdout()) {
        cldm_warn("Could not read captured stdout");
    }

    cldm_log("\nCaptured stderr:");
    if(cldm_io_dump_captured_stderr()) {
        cldm_warn("Could not read captured stderr");
    }

    cldm_io_remove_captured_stdout();
    cldm_io_remove_captured_stderr();


    status = 0;
epilogue:
    if(cldm_stdout) {
        if(cldm_io_restore_stdout()) {
            cldm_warn("Failed to restore stdout");
        }
    }
    if(cldm_stderr) {
        if(cldm_io_restore_stderr()) {
            cldm_warn("Failed to restore stderr");
        }
    }

    return status;
}
