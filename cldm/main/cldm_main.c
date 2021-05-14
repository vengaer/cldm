#include "cldm_argp.h"
#include "cldm_elf.h"
#include "cldm_io.h"
#include "cldm_log.h"
#include "cldm_mock.h"
#include "cldm_parallel.h"
#include "cldm_sequential.h"

int main(int argc, char **argv) {
    int status;
    struct cldm_elfmap map;
    struct cldm_args args;

    /* Prevent mocks from interfering with internals */
    cldm_mock_disable_all();

    if(!cldm_argp_parse(&args, argc, argv)) {
        return 1;
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

    status = 1;

    if(!cldm_is_elf64(&map) || !cldm_elf_is_executable(&map)) {
        cldm_err("%s is not a 64-bit ELF executable", argv[0]);
        goto epilogue;
    }

    if(!cldm_io_capture_stream(args.capture)) {
        goto epilogue;
    }

    if(args.jobs > 1) {
        status = cldm_parallel_run(&map, &args);
    }
    else {
        status = cldm_sequential_run(&map, &args);
    }

    cldm_io_capture_dump(args.capture, args.redirect);

epilogue:
    cldm_unmap_elf(&map);
    cldm_io_capture_restore(args.capture);

    return status;
}
