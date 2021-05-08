#include "cldm_argp.h"
#include "cldm_dl.h"
#include "cldm_elf.h"
#include "cldm_elfdump.h"
#include "cldm_io.h"
#include "cldm_log.h"
#include "cldm_macro.h"
#include "cldm_mock.h"
#include "cldm_runner.h"
#include "cldm_test.h"

#include <stdbool.h>

#include <sys/types.h>

int main(int argc, char **argv) {
    int status;
    struct cldm_elfmap map;
    struct cldm_args args;

    /* Prevent mocks from interfering with internals */
    cldm_mock_force_disable = true;

    if(!cldm_argp_parse(&args, argc, argv)) {
        return -1;
    }

    if(args.help) {
        cldm_argp_usage(argv[0]);
        return 0;
    }
    if(args.version) {
        cldm_argp_version();
        return 0;
    }

    if(cldm_map_elf(&map, argv[0])) {
        cldm_err("Mapping %s failed", argv[0]);
        return -1;
    }

    status = -1;

    if(!cldm_is_elf64(&map) || !cldm_elf_is_executable(&map)) {
        cldm_err("%s is not a 64-bit ELF executable", argv[0]);
        goto epilogue;
    }

    if(!cldm_io_capture_stream(args.capture)) {
        goto epilogue;
    }

    status = cldm_collect_and_run(&map, args.fail_fast, &argv[args.posidx], (unsigned)argc - args.posidx);

    cldm_io_capture_dump(args.capture);

epilogue:
    cldm_unmap_elf(&map);
    cldm_io_capture_restore(args.capture);

    return status;
}
