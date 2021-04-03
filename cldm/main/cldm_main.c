#include "cldm_dl.h"
#include "cldm_elf.h"
#include "cldm_elfdump.h"
#include "cldm_io.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_mock.h"

#include "cldm_rbtree.h"
#include "cldm_test.h"

#include <sys/types.h>

unsigned char cldm_datavar = 0;

int main(int argc, char *argv[argc + 1]) {
    ssize_t ntests;
    int status;
    struct cldm_elfmap map;
    struct cldm_rbtree tests = cldm_rbtree_init();

    cldm_mock_force_disable = true;

    cldm_log("cldm version " cldm_str_expand(CLDM_VERSION));
    cldm_log("Report bugs to vilhelm.engstrom@tuta.io\n");

    status = 2;

    if(cldm_map_elf(&map, argv[0])) {
        cldm_err("Mapping %s failed", argv[0]);
        return 1;
    }

    if(!cldm_is_elf64(&map) || !cldm_elf_is_executable(&map)) {
        cldm_err("%s is not a 64-bit ELF executable", argv[0]);
        goto epilogue;
    }

    if(cldm_io_capture_stdout()) {
        cldm_err("Error redirecting stdout");
        goto epilogue;
    }

    if(cldm_io_capture_stderr()) {
        cldm_err("Error redirecting stderr");
        goto epilogue;
    }

    ntests = cldm_test_collect(&tests, &map);
    if(ntests < 0) {
        cldm_err("Error collecting tests");
        goto epilogue;
    }

    cldm_log("Collected %lld tests", (long long)ntests);
    status = cldm_test_invoke_each(&tests, &map, (size_t)ntests);

    if(cldm_io_dump_captured_stdout()) {
        cldm_warn("Could not read captured stdout");
    }

    if(cldm_io_dump_captured_stderr()) {
        cldm_warn("Could not read captured stderr");
    }

    cldm_io_remove_captured_stdout();
    cldm_io_remove_captured_stderr();

epilogue:
    cldm_unmap_elf(&map);
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
